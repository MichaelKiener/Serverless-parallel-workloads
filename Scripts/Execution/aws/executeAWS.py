import requests
import sys
from datetime import datetime
import os
import time
import json


def callFunction(funcName, input, threads, memory):
    # os.system("aws --version")
    print("Calling: %s", funcName, threads, input)
    with open('payload.json') as f:
        data = json.load(f)
    data["Threads"] = threads
    data["Input"] = input
    with open('payload.json', 'w') as f:
        json.dump(data, f, indent=2)
    res = os.popen('aws lambda invoke --function-name ' + funcName + ' --payload fileb://payload.json output.json --log-type Tail --query "LogResult" --output text |  base64 -D').read()
    f = open("output.json", "r")
    resultOut = f.read()
    res += resultOut
    return res


def updateFunction(funcName, src, runtime, memory, language=""):
    print("Updating function " + funcName + " to memory " + memory + "MB")
    print("aws lambda update-function-configuration --function-name " + funcName + " --memory-size " + memory)
    os.system("aws lambda update-function-configuration --function-name " + funcName + " --memory-size " + memory)

def getFunctionLogs(funcName, limit, since, outputFile):
    return