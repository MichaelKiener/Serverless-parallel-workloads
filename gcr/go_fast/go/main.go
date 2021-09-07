package main

import (
    "fmt"
	"os"
    "kiener_ma/goFast"
    "log"
    "net/http"
    "io/ioutil"
    "encoding/json"
    "runtime"
    "strconv"
)

type InputData struct {
    Input string
    Threads int
}

var invocationCount = 0

func main() {

    /* Testing Vectorization */
/* 
    A := make([]int, 1000)
    B := make([]int, 1000)
    C := make([]int, 1000)

    for i := 0; i<1000; i++ {
        A[i] = i
        B[i] = i * 2
        C[i] = (i + 4) / 2
    }

    for i := 0; i<1000; i++ {
        A[i] = B[i] + C[i];
    }

    fmt.Println(A)
 */

    log.Print("starting server...")
    http.HandleFunc("/", handler)

    // Determine port for HTTP service.
    port := os.Getenv("PORT")
    if port == "" {
            port = "8080"
            log.Printf("defaulting to port %s", port)
    }

    // Start HTTP server.
    log.Printf("listening on port %s", port)
    if err := http.ListenAndServe(":"+port, nil); err != nil {
            log.Fatal(err)
    }
}

func handler(w http.ResponseWriter, r *http.Request) {
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
    // Parse request body
    reqBody, err := ioutil.ReadAll(r.Body)
    if err != nil {
        log.Fatal(err)
    }
    var input InputData
    json.Unmarshal([]byte(reqBody), &input)

    // Run GoFast
    response := go_fast.RunGoFast(input.Input, input.Threads)
    response += "\n" + addInfo
    fmt.Fprintf(w, "%s\n", response)
}
