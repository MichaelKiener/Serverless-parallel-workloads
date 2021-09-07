CC =  g++
CFLAGS = -O3

mvt: mvt.o
	$(CC) $(CFLAGS) -o mvt $+ -lm -fopenmp

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< -fopenmp