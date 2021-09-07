#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <omp.h>
#include <string>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "mandelbrot-set.h"
#include <complex>

double wtime()
{
  struct timeval tv;
  gettimeofday(&tv, 0);

  return tv.tv_sec + 1e-6 * tv.tv_usec;
}



#define CHANNELS 3
// Returns the one dimensional index into our pseudo 3D array
#define OFFSET(y, x, c) (y * x_resolution * CHANNELS + x * CHANNELS + c)

int mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
                    double view_x0, double view_x1, double view_y0, double view_y1,
                    double x_stepsize, double y_stepsize,
                    int palette_shift, unsigned char *img,
                    double power, bool no_output)
{
    using namespace std::complex_literals;

    double y;
    double x;

    std::complex<double> Z;
    std::complex<double> C;

    int k;

    int pointsInSetCount = 0;

// For each pixel in the image
#pragma omp parallel for firstprivate(x, y, k, Z, C) reduction(+ \
                                                               : pointsInSetCount)
    for (int i = 0; i < y_resolution; i++)
    {
        for (int j = 0; j < x_resolution; j++)
        {
            y = view_y1 - i * y_stepsize;
            x = view_x0 + j * x_stepsize;

            Z = 0.0 + 0.0i;
            C = x + y * 1.0i;

            k = 0;

            // Apply the Mandelbrot calculation until the absolute value >= 2 (meaning the calculation will diverge to
            // infinity) or the maximum number of iterations was reached.
            do
            {
                Z = std::pow(Z, power) + C;
                k++;
            } while (std::abs(Z) < 2 && k < max_iter);

            // If the maximum number of iterations was reached then this point is in the Mandelbrot set and we color it
            // black. Else, it is outside and we color it with a color that corresponds to how many iterations there
            // were before we confirmed the divergence.
            if (k == max_iter)
            {
                pointsInSetCount++;
            }

            if (!no_output)
            {
                if (k == max_iter)
                {
                    memcpy(&img[OFFSET(i, j, 0)], black, 3);
                }
                else
                {
                    int index = (k + palette_shift) % (sizeof(colors) / sizeof(colors[0]));
                    // std::cout << index << std::endl;
                    memcpy(&img[OFFSET(i, j, 0)], colors[index], 3);
                }
            }
        }
    }

    return pointsInSetCount;
}

int invocationCount = 0;

using namespace aws::lambda_runtime;

invocation_response my_handler(invocation_request const& request)
{
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
    struct timespec begin, end;
    int x_resolution = 0;
    int y_resolution = 0;

   // return invocation_response::success("Hello, World!", "application/json");
    if (inputParam == "S")
    {
        x_resolution = 256;
        y_resolution = 256;
    }
    else if (inputParam == "M")
    {
        x_resolution = 1024;
        y_resolution = 1024;
    }
    else if (inputParam == "L")
    {
        x_resolution = 3072;
        y_resolution = 3072;
    }
    double power = 2.0;
    int max_iter = 50; // sizeof(colors);

    int palette_shift = 0;
    double view_x0 = -2;
    double view_x1 = +2;
    double view_y0 = -2;
    double view_y1 = +2;
    char file_name[256] = "/tmp/mandelbrot.png";
    int no_output = 0;

    double x_stepsize;
    double y_stepsize;

  // res += "N = " + std::to_string(N);
  int threads = omp_get_num_procs();

  if(thread_input < threads) {
    threads = thread_input;
  }
  omp_set_num_threads(threads);
  FILE *file = nullptr;
    if (!no_output)
    {
        if ((file = fopen(file_name, "w")) == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }

    // This is a pseudo 3d (1d storage) array that can be accessed using the OFFSET Macro.
    auto *image = static_cast<unsigned char *>(malloc(x_resolution * y_resolution * sizeof(unsigned char[3])));

    // Calculate the step size.
    x_stepsize = (view_x1 - view_x0) / (1.0 * x_resolution);
    y_stepsize = (view_y1 - view_y0) / (1.0 * y_resolution);
    if (x_stepsize <= 0 || y_stepsize <= 0)
    {
        printf("Please specify a valid range.\n");
        exit(-1);
    }
    clock_gettime(CLOCK_MONOTONIC, &begin);
    // compute mandelbrot
    int numSamplesInSet = mandelbrot_draw(x_resolution, y_resolution, max_iter,
                                          view_x0, view_x1, view_y0, view_y1,
                                          x_stepsize, y_stepsize, palette_shift, image, power, no_output);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if (!no_output)
    {
        if (fprintf(file, "P6\n%d %d\n%d\n", x_resolution, y_resolution, 255) < 0)
        {
            std::perror("fprintf");
            exit(EXIT_FAILURE);
        }
        size_t bytes_written = fwrite(image, 1,
                                      x_resolution * y_resolution * sizeof(unsigned char[3]), file);
        if (bytes_written < x_resolution * y_resolution * sizeof(char[3]))
        {
            std::perror("fwrite");
            exit(EXIT_FAILURE);
        }

        fclose(file);
    }

    // This is just here for convenience, it isn't actually used to measure your runtime
    double time = (((double)end.tv_sec + 1.0e-9 * end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9 * begin.tv_nsec)) * 1000;
    fprintf(stderr, "\n\nTime: %.5f seconds\n", ((double)end.tv_sec + 1.0e-9 * end.tv_nsec) - ((double)begin.tv_sec + 1.0e-9 * begin.tv_nsec));

  free(image);
  res += "Execution Time: " + std::to_string(time) + "ms,\n Threads: " + std::to_string(threads);
  res += "\n" + addInfo;
  return invocation_response::success(res, "application/json");
}

int main()
{
   run_handler(my_handler);
   return 0;
}