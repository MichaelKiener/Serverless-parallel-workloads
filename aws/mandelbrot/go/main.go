// main.go
package main

import (
	"github.com/aws/aws-lambda-go/lambda"
	"kiener_ma/mandelbrot"
	"fmt"
    "runtime"
    "strconv"
	"io/ioutil"
)

var invocationCount = 0

type FunctionEvent struct {
	Input string `json:"Input"`
	Threads int  `json:"Threads"`
}

type FunctionResponse struct {
	Response string `json:"Response"`
}

func FunctionHandler(event FunctionEvent) (FunctionResponse, error) {
	invocationCount++
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
	fmt.Println(event.Input, event.Threads)
	response := mandelbrot.RunMandelbrot(event.Input, event.Threads)
	// fmt.Println(response)
	response += "\n" + addInfo
	return FunctionResponse{Response: response}, nil
}

func main() {
	// Make the handler available for Remote Procedure Call by AWS Lambda
	lambda.Start(FunctionHandler)
}
