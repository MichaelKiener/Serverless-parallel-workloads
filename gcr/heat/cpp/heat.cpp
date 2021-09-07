#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include "heat.hpp"
#include "mmintrin.h"

// #include <papi.h>

double wtime()
{
  struct timeval tv;
  gettimeofday(&tv, 0);

  return tv.tv_sec + 1e-6 * tv.tv_usec;
}

double relax_jacobi(double **u1, double **utmp1,
					int sizex, int sizey)
{
	int i, j;
	double *help, *u, *utmp, factor = 0.5;

	utmp = *utmp1;
	u = *u1;
	double unew, diff, sum = 0.0;

#pragma omp parallel for reduction(+ \
								   : sum) private(i, j, unew, diff)
	for (i = 1; i < sizey - 1; i++)
	{
		int ii = i * sizex;
		int iim1 = (i - 1) * sizex;
		int iip1 = (i + 1) * sizex;
#pragma ivdep
		for (j = 1; j < sizex - 1; j++)
		{
			unew = 0.25 * (u[ii + (j - 1)] +
						   u[ii + (j + 1)] +
						   u[iim1 + j] +
						   u[iip1 + j]);
			diff = unew - u[ii + j];
			utmp[ii + j] = unew;
			sum += diff * diff;
		}
	}

	*u1 = utmp;
	*utmp1 = u;
	return (sum);
}

std::string runHeat(std::string input, int thread_input)
{
    int i, j;
	int np, iter;
	double time;

	// algorithmic parameters
	algoparam_t param;

	// timing

    double residual;
  std::string inputParam = input;

    int N = 0;

    if (inputParam == "S")
    {
        N = 2200;
    }
    else if (inputParam == "M")
    {
        N = 5200;
    }
    else if (inputParam == "L")
    {
        N = 15200;
    }
	param.initial_res = N;
	param.max_res = N;
    int threads = omp_get_num_procs();
    if (thread_input < threads)
    {
        threads = thread_input;
    }
    omp_set_num_threads(threads);
	// set the visualization resolution
	param.visres = 100;
	// check arguments
	initialize_params(&param);
	initialize(&param);
	// time = (double *) malloc(sizeof(double), (int) (param.max_res - param.initial_res + param.res_step_size) / param.res_step_size);

#pragma omp parallel for
	for (i = 0; i < param.act_res + 2; i++)
	{
		for (j = 0; j < param.act_res + 2; j++)
		{
			param.uhelp[i * (param.act_res + 2) + j] = param.u[i * (param.act_res + 2) + j];
		}
	}
	// starting time
	// time[exp_number] = wtime();
	residual = 999999999;
	np = param.act_res + 2;
	time = wtime();
	for (iter = 0; iter < param.maxiter; iter++)
	{
		residual = relax_jacobi(&(param.u), &(param.uhelp), np, np);
	}

	time = (wtime() - time) * 1000;
	printf("Execution time: %fms, Threads: %d\n", time, threads);

	finalize(&param);
  std::string res = "Execution Time: " + std::to_string(time) + "ms\n Threads: " + std::to_string(threads);
  return res;
}