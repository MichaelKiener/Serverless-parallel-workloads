package heat

import (
	"gonum.org/v1/gonum/mat"
	"time"
	"math"
	"sync"
	"runtime"
	"strconv"
)

type HeatSrc struct {
	posX, posY, size, temp float64
}

func swap(x *mat.Dense, y *mat.Dense) {
	var temp mat.Dense
	temp = *x    /* save the value at address x */
	*x = *y      /* put y into x */
	*y = temp    /* put temp into y */
 }

func RunHeat(size string, threads_in int) string {

	dataSize := size
	iterations := 10
	N := 0
	if dataSize == "S" {
		N = 2200
	}
	if dataSize == "M" {
		N = 5200
	}
	if dataSize == "L" {
		N = 15200
	}

	heatSrcArr := []HeatSrc {
		HeatSrc{
			posX: 0.0,
			posY: 0.0,
			size: 1.0,
			temp: 1.0,
		},
		HeatSrc{
			posX: 1.0,
			posY: 1.0,
			size: 1.0,
			temp: 0.5,
		},
	}
	np := N + 2
	var wg sync.WaitGroup
	threads := 0
	threads = threads_in
	if runtime.NumCPU() < threads {
		threads = runtime.NumCPU()
	}
	start := time.Now()
	// Matrix init
	A := mat.NewDense(np, np, nil)
	A_help := mat.NewDense(np,np, nil)
	for i := 0; i < threads; i++ {
		wg.Add(1)
		go func (index int) {
			lowerBound := index * (np / threads)
			upperBound := (index + 1) * (np / threads)
			// fmt.Println(lowerBound, upperBound)
			// Init heatSrc
			for i, src := range heatSrcArr {
				// Top row
				dummy := i
				dummy++
				for j := lowerBound; j < upperBound; j++ {
					dist := math.Sqrt(math.Pow(float64(j) / float64(np - 1) - src.posX, 2) + math.Pow(src.posY, 2))
					if dist <= src.size {
						val := A.At(0, j) + (src.size - dist) / src.size * src.temp
						A.Set(0, j, val)
					}
				}
				// Bottom row
				for j := lowerBound; j < upperBound; j++ {
					dist := math.Sqrt(math.Pow(float64(j) / float64(np - 1) - src.posX, 2) + math.Pow(1 - src.posY, 2))
					if dist <= src.size {
						val := A.At(np - 1, j) + (src.size - dist) / src.size * src.temp
						A.Set(np - 1, j, val)
					}
				}
				// Leftmost column
				for j := lowerBound + 1; j < upperBound - 1; j++ {
					dist := math.Sqrt(math.Pow(src.posX, 2) + math.Pow(float64(j) / float64(np - 1) - src.posY, 2))
					if dist <= src.size {
						val := A.At(j, 0) + (src.size - dist) / src.size * src.temp
						A.Set(j, 0, val)
					}
				}
				// Rightmost column
				for j := lowerBound + 1; j < upperBound - 1; j++ {
					dist := math.Sqrt(1 - math.Pow(src.posX, 2) + math.Pow(float64(j) / float64(np - 1) - src.posY, 2))
					if dist <= src.size {
						val := A.At(j, np - 1) + (src.size - dist) / src.size * src.temp
						A.Set(j, np - 1, val)
					}
				}
			}
			wg.Done()
		}(i)
	}
	wg.Wait()
	// fmt.Print(A)
	//A_help := mat.DenseCopyOf(A)
	// A_help.Set(0, 0, 5.0)
	residualSum := 0.0
	residuals := make([]float64, threads)
	for iter := 0; iter < iterations; iter++ {
		for i := 0; i < threads; i++ {
			wg.Add(1)
			go func (index int) {
				lowerBound := index * (N / threads) + 1
				upperBound := (index + 1) * (N / threads) + 1
				val, diff, residual_thread := 0.0, 0.0, 0.0
				// fmt.Println(lowerBound, upperBound)
				for row := lowerBound; row < upperBound; row++ {
					for col := 1; col < np - 1; col++ {
						val = 0.25 * (A.At(row - 1, col) + A.At(row + 1, col) + A.At(row, col - 1) + A.At(row, col + 1))
						diff = val - A.At(row, col)
						A_help.Set(row, col, val)
						residual_thread += diff * diff
					}
				}
				residuals[index] = residual_thread
				wg.Done()
			}(i)
		}
		wg.Wait()
		residualSum = 0.0
		for i := range residuals {
			residualSum += residuals[i]
			residuals[i] = 0
		}
		swap(A, A_help)
	}

	duration := time.Since(start)

	timeInMS := float64(duration) / 1000000
	runtime.GC()
	return "Execution Time: " + strconv.FormatFloat(timeInMS, 'f', 6, 64) + "ms \nThreads: " + strconv.Itoa(threads)
}