CC =  g++
CFLAGS = -O3 -Xclang -fopenmp

go_fast: go_fast.o
	$(CC) $(CFLAGS) -o go_fast $+ -lm -lomp 

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<