cmake_minimum_required(VERSION 3.10)

# Define the project name and where to report bugs.
project(cpp-samples-cloud-run-hello-world CXX C)

# Configure the Compiler options, we will be using C++17 features.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS} -fopenmp")

find_package(Boost 1.66 REQUIRED COMPONENTS program_options)
find_package(Threads)


add_executable(monte monte_handler.cpp monte.cpp)
target_link_libraries(
  monte PRIVATE Boost::headers Boost::program_options
                          Threads::Threads)


include(GNUInstallDirs)
install(TARGETS monte RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})