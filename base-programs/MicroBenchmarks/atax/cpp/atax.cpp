#include <iostream>
#include <string>
#include <sys/time.h>
#include "omp.h"
#include <iostream>

double wtime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    return tv.tv_sec + 1e-6 * tv.tv_usec;
}

// Assumes cols = vector size and results
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

int main(int argc, char *argv[])
{

    std::string inputParam = argv[1];

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
    char *p;
    double thread_input = strtol(argv[2], &p, 10);
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
    std::cout << "Execution Time: " << std::to_string(time) << "ms \n Threads: " << std::to_string(threads) << std::endl;
    free(A);
    free(x);
    free(x_help);
}