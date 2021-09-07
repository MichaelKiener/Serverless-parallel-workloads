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
	"kiener_ma/monte"
    "runtime"
    "strconv"
	"io/ioutil"
)

var invocationCount = 0

// HelloHTTP is an HTTP Cloud Function with a request parameter.
func Monte_go(w http.ResponseWriter, r *http.Request) {
	invocationCount++
	addInfo := "InvocationCount: " + strconv.Itoa(invocationCount) + "\n"
	executionID := r.Header.Get("Function-Execution-Id")
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

	var d struct {
		Threads int `json:"Threads"`
		Input string `json:"Input"`
	}
	if err := json.NewDecoder(r.Body).Decode(&d); err != nil {
		fmt.Fprint(w, "Error " + err.Error())
		return
	}

	response := monte.RunMonte(d.Input, d.Threads)
	response += "\n" + addInfo
	response += "\n ExecutionID: " + executionID
	fmt.Fprintf(w, "%s", response)
}

// [END functions_helloworld_http]
