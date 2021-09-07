// main.go
package main

import (
	"github.com/aws/aws-lambda-go/lambda"
	"kiener_ma/atax"
	"fmt"
    "runtime"
    "strconv"
	"io/ioutil"
	"time"
)

var invocationCount = 0

type FunctionEvent struct {
	Input string `json:"Input"`
	Threads int  `json:"Threads"`
}

type FunctionResponse struct {
	Response string `json:"Response"`
}

func PrintMemUsage() {
    var m runtime.MemStats
    runtime.ReadMemStats(&m)
    // For info on each, see: https://golang.org/pkg/runtime/#MemStats
    fmt.Printf("Alloc = %v MiB", bToMb(m.Alloc))
    fmt.Printf("\tTotalAlloc = %v MiB", bToMb(m.TotalAlloc))
    fmt.Printf("\tSys = %v MiB", bToMb(m.Sys))
    fmt.Printf("\tNumGC = %v\n", m.NumGC)
}

func bToMb(b uint64) uint64 {
return b / 1024 / 1024
}

func FunctionHandler(event FunctionEvent) (FunctionResponse, error) {
	start := time.Now()
	invocationCount++
	// PrintMemUsage()
	addInfo := "InvocationCount: " + strconv.Itoa(invocationCount) + "\n"
	if invocationCount <= 1 {
		cpuInfo, errFile := ioutil.ReadFile("/proc/cpuinfo")

		if errFile != nil {
			fmt.Println(errFile.Error())
		} else {
			// fmt.Println(string(cpuInfo))
			addInfo += "Hostname: " + string(cpuInfo)
		}

    	cpus := runtime.NumCPU()
    	addInfo += "CPU count: " + strconv.Itoa(cpus) + "\n"
	}
	// log.Printf(addInfo)
    // fmt.Println(addInfo)
	fmt.Println(runtime.Version())
	fmt.Println(event.Input, event.Threads)
	response := atax.RunAtax(event.Input, event.Threads)
	response += "\n Started at " + start.String()
	// fmt.Println(response)
	response += "\n" + addInfo
	// PrintMemUsage()
	return FunctionResponse{Response: response}, nil
}

func main() {
	// Make the handler available for Remote Procedure Call by AWS Lambda
	lambda.Start(FunctionHandler)
}
