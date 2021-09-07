package atax

import (
	"time"
	"sync"
	"runtime"
	"strconv"
)

func RunAtax(size string, threads_in int) string {
	dataSize := size
	N := 0
	M := 0
	fn := 5000.0
	if dataSize == "S" {
		M = 2000
		N = 2500
	}
	if dataSize == "M" {
		M = 4000
		N = 5000
	}
	if dataSize == "L" {
		M = 20000
		N = 22000
	}
	A := make([]float64, N * M)
	x := make([]float64, N)
	x_help := make([]float64, N)

	threads := 0
	threads = threads_in
	if runtime.NumCPU() < threads {
		threads = runtime.NumCPU()
	}
	var wg sync.WaitGroup
	// Init Matrices / Arrays
	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (N / threads)
			upperBound := (index + 1) * (N / threads)
			for vec_Index := lowerBound; vec_Index < upperBound; vec_Index++ {
				x[vec_Index] = 1.0 + float64(vec_Index) / float64(fn);
			}
			lowerBound = index * (M / threads)
			upperBound = (index + 1) * (M / threads)
			for row := lowerBound; row < upperBound; row++ {
				for col := 0; col < N; col++ {
					A[row * N + col] = float64(((row + col) % N)) / float64((5 * M));
				}
			}
			wg.Done()
		}(i)
	}
	wg.Wait()

	start := time.Now()
	// A @ x
	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (M / threads)
			upperBound := (index + 1) * (M / threads)
			for row := lowerBound; row < upperBound; row++ {
				for col := 0; col < N; col++ {
					x_help[row] += A[row * N + col] * x[col];
				}
			}
			wg.Done()
		}(i)
	}
	wg.Wait()
	// (A @ x) @ x
	for i:= 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (M / threads)
			upperBound := (index + 1) * (M / threads)
			for row := lowerBound; row < upperBound; row++ {
				for col := 0; col < N; col ++ {
					x[row] += A[row * N + col] * x_help[col];
				}
			}
			wg.Done()
		}(i)
	}
	wg.Wait()

	duration := time.Since(start)

	timeInMS := float64(duration) / 1000000
	runtime.GC()
	return "Execution Time: " + strconv.FormatFloat(timeInMS, 'f', 6, 64) + "ms \nThreads: " + strconv.Itoa(threads)
}