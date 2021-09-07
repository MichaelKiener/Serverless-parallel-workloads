CC =  g++
CFLAGS = -O3 

atax: atax.o
	$(CC) $(CFLAGS) -o atax $+ -lm -fopenmp

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< -fopenmp