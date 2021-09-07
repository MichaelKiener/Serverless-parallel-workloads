#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#define SEED 35791246

double wtime()
{
  struct timeval tv;
  gettimeofday(&tv, 0);

  return tv.tv_sec + 1e-6 * tv.tv_usec;
}

int main(int argc, char *argv[])
{
 double time = wtime();
  std::string inputParam = argv[1];

  int N = 0;

  if (inputParam == "S")
  {
    N = 2000;
  }
  else if (inputParam == "M")
  {
    N = 6000;
  }
  else if (inputParam == "L")
  {
    N = 22000;
  }


  int threads = omp_get_max_threads();
  char *p;
  double thread_input = strtol(argv[2], &p, 10);
  if(thread_input < threads) {
    threads = thread_input;
  }
  omp_set_num_threads(threads);

  double *A;
  A = (double *)malloc(N * N * sizeof(double *));
  unsigned int state[threads];

   for (int i = 0; i < threads; i++) {
      state[i] = 35791246 * threads;
   }
  srand(SEED);
  // Initialize Arrays
  #pragma omp parallel for firstprivate(state)
  for (int i = 0; i < N * N; i++)
  {
    A[i] = (double)rand_r(&state[omp_get_thread_num()])/RAND_MAX;
  }

  // Computation

  double trace = 0.0;
#pragma omp parallel for reduction(+ \
                                   : trace)
  for (int i = 0; i < N; i++)
  {
    trace += tanh(A[i + i * N]);
  }

#pragma omp parallel for firstprivate(trace)
  for (int i = 0; i < N * N; i++)
  {
    A[i] = A[i] + trace;
  }
  time = wtime() - time;
  time = time * 1000;
  std::cout << "Execution Time: "<< std::to_string(time) << "ms \n Threads: " << std::to_string(threads) << std::endl;
  free(A);
  return 0;
}