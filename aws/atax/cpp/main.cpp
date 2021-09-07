#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#define SEED 35791246

double wtime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    return tv.tv_sec + 1e-6 * tv.tv_usec;
}

void vector_mult_matrix(double *matrix, double *vec, double *result, int rows, int cols)
{
#pragma omp parallel for
    for (int row_matrix = 0; row_matrix < rows; row_matrix++)
    {
        for (int col_matrix = 0; col_matrix < cols; col_matrix++)
        {
            result[row_matrix] += matrix[row_matrix * cols + col_matrix] * vec[col_matrix];
        }
    }
    return;
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
        // res += "Threads existing " + std::to_string(v.GetInteger("Threads")) + " ";
    }
    if (v.ValueExists("Input") && v.GetObject("Input").IsString())
    {
        inputParam = v.GetString("Input");
        // res += "Input existing " + v.GetString("Input") + " ";
    }

    int N = 0;
    int M = 0;
    double fn = 5000.0;
    if (inputParam == "S")
    {
        M = 2000;
        N = 2500;
    }
    else if (inputParam == "M")
    {
        M = 4000;
        N = 5000;
    }
    else if (inputParam == "L")
    {
        M = 20000;
        N = 22000;
    }
    int threads = omp_get_num_procs();
    if (thread_input < threads)
    {
        threads = thread_input;
    }
    omp_set_num_threads(threads);
    double *A, *x, *x_help;
    A = (double *)malloc(M * N * sizeof(double *));
    x = (double *)malloc(N * sizeof(double *));
    x_help = (double *)malloc(N * sizeof(double *));

// Initialize x
#pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        x[i] = 1.0 + (double)i / fn;
    }

// Initialize A
#pragma omp parallel for
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i * N + j] = (double)((i + j) % N) / (5.0 * (double)M);
        }
    }
    double time = wtime();
    vector_mult_matrix(A, x, x_help, M, N);
    vector_mult_matrix(A, x_help, x, M, N);
    time = wtime() - time;
    time = time * 1000;
    free(A);
    free(x);
    free(x_help);
    res += "Execution Time: " + std::to_string(time) + "ms,\n Threads: " + std::to_string(threads);
    res += "\n" + addInfo;
    return invocation_response::success(res, "application/json");
}

int main()
{
    run_handler(my_handler);
    return 0;
}