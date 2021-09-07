CC =  g++
CFLAGS = -O3 -Xclang -fopenmp

MPICC = mpicc

all: heat

heat : heat.o input.o misc.o timing.o relax_jacobi.o
	$(CC) $(CFLAGS) -o heat $+ -lm -lomp

%.o : %.cpp %.hpp %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o heat *~ *.ppm

remake : clean all
