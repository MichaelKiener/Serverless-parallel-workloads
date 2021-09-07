#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include "mvt.hpp"

#define SEED 35791246

double wtime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    return tv.tv_sec + 1e-6 * tv.tv_usec;
}

std::string runMVT(std::string input, int thread_input)
{

    std::string inputParam = input;

    int N = 0;

    if (inputParam == "S")
    {
        N = 2000;
    }
    else if (inputParam == "M")
    {
        N = 5500;
    }
    else if (inputParam == "L")
    {
        N = 18000;
    }

    int threads = omp_get_num_procs();
    if (thread_input < threads)
    {
        threads = thread_input;
    }
    omp_set_num_threads(threads);
    double *A, *x1, *x2, *y1, *y2;
    A = (double *)malloc(N * N * sizeof(double *));
    x1 = (double *)malloc(N * sizeof(double *));
    x2 = (double *)malloc(N * sizeof(double *));
    y1 = (double *)malloc(N * sizeof(double *));
    y2 = (double *)malloc(N * sizeof(double *));

// Initialize
#pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        x1[i] = (double)(i % N) / (double)N;
        x2[i] = (double)((i + 1) % N) / (double)N;
        y1[i] = (double)((i + 3) % N) / (double)N;
        y2[i] = (double)((i + 4) % N) / (double)N;
        for (int j = 0; j < N; j++)
        {
            A[i * N + j] = (double)((i * j) % N) / ((double)N);
        }
    }

    double time = wtime();

#pragma omp parallel for
    for (int rows = 0; rows < N; rows++)
    {
        for (int cols = 0; cols < N; cols++)
        {
            x1[rows] += A[rows * N + cols] * y1[cols];
            x2[rows] += A[cols * N + rows] * y2[cols];
        }
    }

    time = wtime() - time;
    time = time * 1000;
    printf("Executiontime: %gms \n", time);
    free(A);
    free(x1);
    free(x2);
    free(y1);
    free(y2);
    std::string res = "Execution Time: " + std::to_string(time) + "ms\n Threads: " + std::to_string(threads);
    return res;
}