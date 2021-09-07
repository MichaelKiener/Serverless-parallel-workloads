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

  // res += "N = " + std::to_string(N);
  int threads = omp_get_num_procs();

  if (thread_input < threads)
  {
    threads = thread_input;
  }
  omp_set_num_threads(threads);
  double *A;
  A = (double *)malloc(N * N * sizeof(double *));

  srand(SEED);
  // Initialize Arrays
  // #pragma omp parallel for
  for (int i = 0; i < N * N; i++)
  {
    A[i] = (double)rand() / RAND_MAX;
  }
  printf("Initialised\n");
  // Computation
  double time = wtime();
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
  printf("Trace: %f\n", trace);
  printf("Executiontime: %gs \n", time);
  free(A);
  res += "Execution Time: " + std::to_string(time) + "ms,\n Threads: " + std::to_string(threads);
  res += "\n" + addInfo;
  return invocation_response::success(res, "application/json");
}

int main()
{
  run_handler(my_handler);
  return 0;
}