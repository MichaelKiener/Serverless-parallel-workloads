#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <random>

double wtime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    return tv.tv_sec + 1e-6 * tv.tv_usec;
}

int invocationCount = 0;

using namespace aws::lambda_runtime;

invocation_response my_handler(invocation_request const &request)
{
    invocationCount++;
    std::string addInfo = "Invocation Count: " + std::to_string(invocationCount);
    if (invocationCount <= 1)
    {
        std::string cpuInfo = "";
        {
            using namespace std;
            char ch;
            //creating the object of the data type 'istream'
            ifstream new_file;
            //opening the above file
            new_file.open("/proc/cpuinfo", ios::in);
            //checking whether the file is available or not
            if (!new_file)
            {
                cout << "Sorry the file you are looking for is not available" << endl;
            }
            else
            {
                // reading the whole file till the end
                std::string temp;
                while (std::getline(new_file, temp))
                {
                    cpuInfo += temp + "\n";
                }
                //closing the file after reading
                new_file.close();
            }
            addInfo += "\n" + cpuInfo;
        }
    }
    using namespace Aws::Utils::Json;
    std::string res = "";
    JsonValue json(request.payload);
    if (!json.WasParseSuccessful())
    {
        return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
    }

    auto v = json.View();
    int thread_input = 10;
    std::string inputParam = "";
    if (v.ValueExists("Threads") && v.GetObject("Threads").IsIntegerType())
    {
        thread_input = v.GetInteger("Threads");
    }
    if (v.ValueExists("Input") && v.GetObject("Input").IsString())
    {
        inputParam = v.GetString("Input");
    }
    double time;
    long long niter = 0;
    double x, y;
    long long count = 0; /* # of points in the 1st quadrant of unit circle */
    double z;
    double pi;
    if (inputParam == "S")
    {
        niter = 10000000;
    }
    else if (inputParam == "M")
    {
        niter = 100000000;
    }
    else if (inputParam == "L")
    {
        niter = 1000000000;
    }
    int threads = omp_get_num_procs();

    if (thread_input < threads)
    {
        threads = thread_input;
    }
    omp_set_num_threads(threads);
    unsigned int state[threads];

    for (int i = 0; i < threads; i++)
    {
        state[i] = 35791246 * threads;
    }

    /* initialize random numbers */
    count = 0;
    int threadIndex = 1;
    time = wtime();

#pragma omp parallel for private(x, y, z) firstprivate(state) reduction(+ \
                                                                        : count)
    for (int i = 0; i < niter; i++)
    {

        x = (double)rand_r(&state[omp_get_thread_num()]) / RAND_MAX;
        y = (double)rand_r(&state[omp_get_thread_num()]) / RAND_MAX;

        z = x * x + y * y;
        if (z <= 1)
            count++;
    }
    pi = (double)count / niter * 4;
    time = wtime() - time;
    res += "Execution Time: " + std::to_string(time) + "ms,\n Threads: " + std::to_string(threads);
    res += "\n" + addInfo;
    return invocation_response::success(res, "application/json");
}

int main()
{
    run_handler(my_handler);
    return 0;
}