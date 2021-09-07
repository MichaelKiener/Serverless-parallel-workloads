package mvt
import (
	"time"
	"sync"
	"runtime"
	"strconv"
)

func RunMVT(size string, threads_in int) string {
	dataSize := size
	N := 0
	if dataSize == "S" {
		N = 2000
	}
	if dataSize == "M" {
		N = 5500
	}
	if dataSize == "L" {
		N = 18000
	}
	A := make([]float64, N * N)
	x1 := make([]float64, N)
	x2 := make([]float64, N)
	y1 := make([]float64, N)
	y2 := make([]float64, N)

	threads := 0
	threads = threads_in
	if runtime.NumCPU() < threads {
		threads = runtime.NumCPU()
	}
	var wg sync.WaitGroup

	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (N / threads)
			upperBound := (index + 1) * (N / threads)
			for row := lowerBound; row < upperBound; row++ {
				x1[row] = float64((row % N)) / float64(N)
				x2[row] = float64(((row + 1) % N)) / float64(N)
				y1[row] = float64(((row + 3) % N)) / float64(N)
				y2[row] = float64(((row + 4) % N)) / float64(N)
				for col := 0; col < N; col++ {
					A[row * N + col] = float64(((row * col) % N)) / float64(N);
				}
			}
			wg.Done()
		}(i)
	}
	wg.Wait()

	start := time.Now()
	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (N / threads)
			upperBound := (index + 1) * (N / threads)
			for row := lowerBound; row < upperBound; row++ {
				for col := 0; col < N; col++ {
					//  x1 += A @ y_1
					x1[row] += A[row * N + col] * y1[col];
					// x2 += y_2 @ A
					x2[row] +=  y2[col] *  A[col * N + row];
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