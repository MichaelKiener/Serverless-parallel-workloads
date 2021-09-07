CC =  g++
CFLAGS = -O3

monte: monte_carlo.o
	$(CC) $(CFLAGS) -o monte $+ -lm -fopenmp

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< -fopenmp
