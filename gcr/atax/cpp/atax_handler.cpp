#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <thread>
#include "atax.hpp"

int invocationCount = 0;

namespace pt = boost::property_tree;
namespace be = boost::beast;
namespace asio = boost::asio;
namespace po = boost::program_options;
using tcp = boost::asio::ip::tcp;

std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

po::variables_map parse_args(int &argc, char *argv[])
{
    // Initialize the default port with the value from the "PORT" environment
    // variable or with 8080.
    auto port = [&]() -> std::uint16_t
    {
        auto env = std::getenv("PORT");
        if (env == nullptr)
            return 8080;
        auto value = std::stoi(env);
        if (value < std::numeric_limits<std::uint16_t>::min() ||
            value > std::numeric_limits<std::uint16_t>::max())
        {
            std::ostringstream os;
            os << "The PORT environment variable value (" << value
               << ") is out of range.";
            throw std::invalid_argument(std::move(os).str());
        }
        return static_cast<std::uint16_t>(value);
    }();

    // Parse the command-line options.
    po::options_description desc("Server configuration");
    desc.add_options()
        //
        ("help", "produce help message")
        //
        ("address", po::value<std::string>()->default_value("0.0.0.0"),
         "set listening address")
        //
        ("port", po::value<std::uint16_t>()->default_value(port),
         "set listening port");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help"))
    {
        std::cout << desc << "\n";
    }
    return vm;
}

int main(int argc, char *argv[])
try
{
    // auto cpuInfo = exec("lscpu");
    // std::cout << cpuInfo << std::endl;
    std::string cpuInfo = "";
    {
        using namespace std;
        //creating the object of the data type 'istream'
        ifstream new_file;
        //opening the above file
        new_file.open("/proc/cpuinfo", ios::in);
        //checking whether the file is available or not
        if (!new_file)
        {
            cout << "Sorry the file you are looking for is not available" << endl;
            return -1;
        }
        else
        {
            // reading the whole file till the end
             std::string temp;
            while(std::getline(new_file, temp)) {
                cpuInfo += temp + "\n";
            }
            //closing the file after reading
            new_file.close();
        }
    }

    std::cout << cpuInfo << std::endl;

    po::variables_map vm = parse_args(argc, argv);

    if (vm.count("help"))
        return 0;

    auto address = asio::ip::make_address(vm["address"].as<std::string>());
    auto port = vm["port"].as<std::uint16_t>();
    std::cout << "Listening on " << address << ":" << port << std::endl;

    auto handle_session = [](tcp::socket socket)
    {
        auto report_error = [](be::error_code ec, char const *what)
        {
            std::cerr << what << ": " << ec.message() << "\n";
        };

        be::error_code ec;
        for (;;)
        {
            be::flat_buffer buffer;

            // Read a request
            be::http::request<be::http::string_body> request;
            be::http::read(socket, buffer, request, ec);
            if (ec == be::http::error::end_of_stream)
                break;
            if (ec)
                return report_error(ec, "read");
            std::cout << "Request: " << request.body() << std::endl;
            pt::ptree root;
            std::stringstream ss;
            ss << request.body();
            pt::read_json(ss, root);
            int thread_amount = root.get<int>("Threads");
            std::string input = root.get<std::string>("Input");
            std::string res = runAtax(input, thread_amount);
            // Send the response
            // Respond to any request with a "Hello World" message.
            be::http::response<be::http::string_body> response{be::http::status::ok,
                                                               request.version()};
            response.set(be::http::field::server, BOOST_BEAST_VERSION_STRING);
            response.set(be::http::field::content_type, "text/plain");
            response.keep_alive(request.keep_alive());
            invocationCount++;
            res += "\nInvocation Count: " + std::to_string(invocationCount);
            /*       auto const* target = std::getenv("TARGET");
      greeting += target == nullptr ? "World" : target; */
            response.body() = std::move(res);
            response.prepare_payload();
            be::http::write(socket, response, ec);
            if (ec)
                return report_error(ec, "write");
        }
        socket.shutdown(tcp::socket::shutdown_send, ec);
    };

    asio::io_context ioc{/*concurrency_hint=*/1};
    tcp::acceptor acceptor{ioc, {address, port}};
    for (;;)
    {
        auto socket = acceptor.accept(ioc);
        if (!socket.is_open())
            break;
        // Run a thread per-session, transferring ownership of the socket
        std::thread{handle_session, std::move(socket)}.detach();
    }

    return 0;
}
catch (std::exception const &ex)
{
    std::cerr << "Standard exception caught " << ex.what() << '\n';
    return 1;
}