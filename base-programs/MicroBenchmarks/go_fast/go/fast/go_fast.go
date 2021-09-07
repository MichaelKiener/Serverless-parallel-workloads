package go_fast

import (
	"fmt"

	"math/rand"
	"math"
	"time"
	"sync"
	"runtime"
	"strconv"
)

// "gonum.org/v1/gonum/mat"
func RunGoFast(size string, threads_in int) string {
	dataSize := size
	N := 0
	if dataSize == "S" {
		N = 2000
	}
	if dataSize == "M" {
		N = 6000
	}
	if dataSize == "L" {
		N = 22000
	}
	data := make([]float64, N * N)
/* 	for i := range data {
		data[i] = rand.NormFloat64()
	} */
	// fmt.Println(data)

	// a := mat.NewDense(N, N, data)

	start := time.Now()
	threads := 0
	threads = threads_in
	if runtime.NumCPU() < threads {
		threads = runtime.NumCPU()
	}
	var wg sync.WaitGroup
	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			source := rand.NewSource(time.Now().UnixNano() * int64(index))
    		generator := rand.New(source)
			lowerBound := index * (N / threads)
			upperBound := (index + 1) * (N / threads)
			for j:= lowerBound; j < upperBound; j++ {
				for k := 0; k < upperBound; k++ {
					data[j * N + k] = generator.Float64()
				}
			}
			wg.Done()
		}(i)
	}
	wg.Wait()
	fmt.Println(threads)

	trace := make([]float64, threads)
	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (N / threads)
			upperBound := (index + 1) * (N / threads)
			trace_thread := 0.0
			for j:= lowerBound; j < upperBound; j++ {
				// trace_thread += math.Tanh(a.At(j, j))
				trace_thread += math.Tanh(data[j * N + j])
			}
			trace[index] = trace_thread
			wg.Done()
		}(i)
	}
	wg.Wait()
	traceSum := 0.0
	for i := range trace {
		traceSum += trace[i]
	}
	for i:= 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (N / threads)
			upperBound := (index + 1) * (N / threads)
			for j:= lowerBound; j < upperBound; j++ {
				for k := 0; k < N; k++ {
					// a.Set(j,k,a.At(j,k) + traceSum)
					data[j * N + k] += traceSum
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