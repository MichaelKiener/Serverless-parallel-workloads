CC =  g++
CFLAGS = -O3 -fopenmp -std=c++17 -Wall -Wextra

mandelbrot: mandelbrot.cpp
	$(CC) $(CFLAGS) -o mandelbrot -O3 -g mandelbrot.cpp -lm


clean:
	rm -f mandelbrot mandelbrot.ppm
