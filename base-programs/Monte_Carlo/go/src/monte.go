package monte

import (
	"time"
	"sync"
	"runtime"
	"strconv"
	"math/rand"
)

func RunMonte(size string, threads_in int) string {
	dataSize := size
	iter := 0
	if dataSize == "S" {
		iter = 10000000
	}
	if dataSize == "M" {
		iter = 100000000
	}
	if dataSize == "L" {
		iter = 1000000000
	}

	threads := 0
	threads = threads_in
	if runtime.NumCPU() < threads {
		threads = runtime.NumCPU()
	}
	counts := make([]int64, threads)
	var wg sync.WaitGroup
	start := time.Now()
	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			source := rand.NewSource(time.Now().UnixNano() * int64(index))
    		generator := rand.New(source)
			x := 0.0
			y := 0.0
			z := 0.0
			lowerBound := index * (iter / threads)
			upperBound := (index + 1) * (iter / threads)
			count_thread := int64(0)
			for j := lowerBound; j < upperBound; j++ {
				//x := rand.NormFloat64()
				//y := rand.NormFloat64()
				x = generator.Float64()
				y = generator.Float64()
				// math.Pow slows down the program by a factor of 10
				//z = math.Pow(x, 2.0) + math.Pow(y, 2.0)
				z = x * x + y * y
				if z <= 1.0 {
					count_thread++
				}
			}
			counts[index] = count_thread
			wg.Done()
		}(i)
	}
	wg.Wait()
	countSum := int64(0);
	for i := 0; i < threads; i++ {
		countSum += counts[i]
	}

	pi := 4.0 * float64(countSum) / float64(iter);
	duration := time.Since(start)
	// pi has to be used somewhere..
	pi += 1
	timeInMS := float64(duration) / 1000000
	runtime.GC()
	return "Execution Time: " + strconv.FormatFloat(timeInMS, 'f', 6, 64) + "ms \nThreads: " + strconv.Itoa(threads)
}