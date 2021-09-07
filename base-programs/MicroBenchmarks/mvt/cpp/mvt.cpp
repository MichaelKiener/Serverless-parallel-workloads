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

void matrix_mult_vector(double *matrix, double *vec, double *result, int rows, int cols)
{
#pragma omp parallel for
    for (int row_matrix = 0; row_matrix < rows; row_matrix++)
    {
        for (int col_matrix = 0; col_matrix < cols; col_matrix++)
        {
            result[row_matrix] += matrix[row_matrix * row_matrix + col_matrix] * vec[col_matrix];
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    std::string inputParam = argv[1];

    printf("Input: %s \n", inputParam.c_str());

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

    int threads = omp_get_max_threads();
    char *p;
    double thread_input = strtol(argv[2], &p, 10);
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

    /*     vector_mult_matrix(A, y1, x1, N, N);

    matrix_mult_vector(A, y2, x2, N, N); */

    time = wtime() - time;
    time = time * 1000;
    std::cout << "Execution Time: " << std::to_string(time) << "ms \n Threads: " << std::to_string(threads) << std::endl;
    free(A);
    free(x1);
    free(x2);
    free(y1);
    free(y2);
}