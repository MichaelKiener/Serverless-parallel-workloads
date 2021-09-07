// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// [START functions_helloworld_http]

// Package helloworld provides a set of Cloud Functions samples.
package helloworld

import (
	"encoding/json"
	"fmt"
	"net/http"
	"kiener_ma/atax"
	"strconv"
	"os/exec"
	"io/ioutil"
	"runtime"
	"time"
)

var invocationCount = 0

// HelloHTTP is an HTTP Cloud Function with a request parameter.
func Atax_go(w http.ResponseWriter, r *http.Request) {
	start := time.Now()
	invocationCount++
	addInfo := "InvocationCount: " + strconv.Itoa(invocationCount) + "\n"
	executionID := r.Header.Get("Function-Execution-Id")
	if invocationCount <= 1 {

		out, err := exec.Command("lscpu").Output()

    // if there is an error with our execution
    // handle it here
    if err != nil {
        fmt.Printf("%s", err)
    }
    // as the out variable defined above is of type []byte we need to convert
    // this to a string or else we will see garbage printed out in our console
    // this is how we convert it to a string
    fmt.Println("Command Successfully Executed")
    output := string(out[:])
    fmt.Println(output)


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
	var d struct {
		Threads int `json:"Threads"`
		Input string `json:"Input"`
	}
	if err := json.NewDecoder(r.Body).Decode(&d); err != nil {
		fmt.Fprint(w, "Error " + err.Error())
		return
	}

	response := atax.RunAtax(d.Input, d.Threads)
	response += "\n" + addInfo
	response += "\n Started at " + start.String()
	response += "\n ExecutionID: " + executionID
	fmt.Fprintf(w, "%s", response)
}

// [END functions_helloworld_http]
