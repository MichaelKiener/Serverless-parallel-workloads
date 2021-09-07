#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include "monte.hpp"

#define SEED 35791246

double wtime()
{
  struct timeval tv;
  gettimeofday(&tv, 0);

  return tv.tv_sec + 1e-6 * tv.tv_usec;
}

std::string runMonte(std::string input, int thread_input)
{
  double time;
  long long niter = 0;
  double x, y;
  long long count = 0; /* # of points in the 1st quadrant of unit circle */
  double z;
  double pi;

  std::string inputParam = input;

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
  srand(SEED);
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
  time = (wtime() - time) * 1000;
  std::string res = "Execution Time: " + std::to_string(time) + "ms\n Threads: " + std::to_string(threads);
  return res;
}