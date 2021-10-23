import requests
import sys
from datetime import datetime, timedelta
import os
import time
import json
import re


if(sys.argv[1] == 'aws'):
    sys.path.insert(1, './aws')
    import executeAWS as executor
elif (sys.argv[1] == 'google'):
    sys.path.insert(1, './google')
    import executegcf as executor
elif (sys.argv[1] == 'gcr'):
    sys.path.insert(1, './gcr')
    import executegcr as executor
elif (sys.argv[1] == 'ow'):
    sys.path.insert(1, './ow')
    import executeOW as executor

functionNames = json.load(open('./functionNames.json', 'r'))
functionName = functionNames[sys.argv[2]][sys.argv[3]]

funcSrc = json.load(open('./funcSrc.json', 'r'))
src = funcSrc[sys.argv[1]][sys.argv[2]][sys.argv[3]]



funcRuntime = json.load(open('./runtimes.json', 'r'))
run = funcRuntime[sys.argv[1]][sys.argv[3]]

# thread_nums = [1, 10]
thread_nums = [1, 6]
inputs_large= ['M', 'L']
inputs_small = ['S', 'M']
# inputs = ['S', 'M']
memConfigs = [512, 2048, 4096, 8192]
# memConfigs = [10240]
# thread_nums = [1]
inputs = ['S']
inputs_small = ['M']
inputs_large = ['M']
#thread_nums = [6]
# memConfigs = [4096]

# Rules change for VM execution
if(sys.argv[1] == 'vm'):
    sys.path.insert(1, './vm')
    import executeVM as executor
    temp = functionName
    functionName = src
    memConfigs = [30000]
    thread_nums = [1, 2, 4, 5, 6]
    inputs_large = ['S', 'M']
    inputs_small = inputs_large
# memConfigs = [4096, 8192]
dictionaryArray = []
""" if sys.argv[1] == 'google':
    try:
        res = os.popen('gcloud functions describe ' + functionName).read()
        max_mem = re.search('availableMemoryMb: (.+?)\n', res).group(1)
        resultsDictionary['memory'] = max_mem
    except AttributeError:
        print("Parsing error") """


for mem in memConfigs:
    resultsDictionary = {'service': sys.argv[1], 'function': sys.argv[2], 'runtime': sys.argv[3], 'memory': mem}
    executor.updateFunction(functionName, src, run, str(mem), sys.argv[3])
    time.sleep(10)
    inputs = []
    if mem < 4096:
        inputs = inputs_small
    else:
        inputs = inputs_large
    print("Starting executions for memory " + str(mem) + "MB")
    for input in inputs:
        resultsDictionary[input] = {}
        for thread in thread_nums:
            resultsDictionary[input]
            resArray = []
            for i in range(0, 10):
                dt_start = (datetime.now() - timedelta(hours=2)).isoformat()
                res = executor.callFunction(functionName, input, thread, mem)
                print(res)
                dt_end = (datetime.now() - timedelta(hours=2)).isoformat()
                print(dt_start, dt_end)
                resArray.append({"Result": res, "Start": dt_start, "End": dt_end})
                time.sleep(10)
                # os.system("gcloud functions logs read " + funcName + " --start-time=" + dt_start + " --end-time=" + dt_end + " > logs.json")
            resultsDictionary[input][thread] = resArray
    dictionaryArray.append(resultsDictionary)
print(dictionaryArray)
jsonString = json.dumps(dictionaryArray, indent=6)

# print(jsonString)
if(sys.argv[1] == 'vm'):
    functionName = temp

file = open('results/' + sys.argv[1] + '/results_' + sys.argv[1] + '_' + functionName + '.json', 'w+')
file.write(jsonString)
time.sleep(0)
executor.getFunctionLogs(functionName, '5000', dt_start, 'results/' + sys.argv[1] + '/results_' + sys.argv[1] + '_' + functionName + '_logs.json')