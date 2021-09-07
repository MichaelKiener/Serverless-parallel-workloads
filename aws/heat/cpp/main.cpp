#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "omp.h"
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

int invocationCount = 0;

using namespace aws::lambda_runtime;

invocation_response my_handler(invocation_request const& request)
{
  int i, j;
	int np, iter;
	double time, residual;

	// algorithmic parameters
	algoparam_t param;
  invocationCount++;
  std::string addInfo = "Invocation Count: " + std::to_string(invocationCount);
  if (invocationCount <= 1) {
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
            while(std::getline(new_file, temp)) {
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
    if (!json.WasParseSuccessful()) {
        return invocation_response::failure("Failed to parse input JSON", "InvalidJSON");
    }

   auto v = json.View();
   int thread_input = 10;
   std::string inputParam = "";
    if (v.ValueExists("Threads") && v.GetObject("Threads").IsIntegerType()) {
        thread_input = v.GetInteger("Threads");
        // res += "Threads existing " + std::to_string(v.GetInteger("Threads")) + " ";
    }
    if (v.ValueExists("Input") && v.GetObject("Input").IsString()) {
         inputParam = v.GetString("Input");
         // res += "Input existing " + v.GetString("Input") + " ";
    }

   // return invocation_response::success("Hello, World!", "application/json");
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
  // res += "N = " + std::to_string(N);
  int threads = omp_get_num_procs();

  if(thread_input < threads) {
    threads = thread_input;
  }
  omp_set_num_threads(threads);
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
	residual = 999999999;
	np = param.act_res + 2;
	time = wtime();
	for (iter = 0; iter < param.maxiter; iter++)
	{
		residual = relax_jacobi(&(param.u), &(param.uhelp), np, np);
	}

	time = (wtime() - time) * 1000;
	finalize(&param);
  res += "Execution Time: " + std::to_string(time) + "ms,\n Threads: " + std::to_string(threads);
  res += "\n" + addInfo;
  return invocation_response::success(res, "application/json");
}

int main()
{
   run_handler(my_handler);
   return 0;
}