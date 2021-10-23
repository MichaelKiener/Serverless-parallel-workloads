import requests
from requests.structures import CaseInsensitiveDict
import sys
from datetime import datetime
import os
import time
import json


def callFunction(funcName, input, threads, memory):
    # os.system("gcloud -v")
    print("Calling: " + funcName + " with input " + input + " and " + str(threads) + " threads")
    url = "https://us-central1-ma-stateful-serverless.cloudfunctions.net/" + funcName
    headers = CaseInsensitiveDict()
    headers["Content-Type"] = "application/json"
    data = {
        "Input": input,
        "Threads": threads
    }
    r = requests.post(url, data=json.dumps(data, indent=2), headers=headers)
    print('Status: ', r.status_code)
    print('Response:', r.content)
    return r.content


def updateFunction(funcName, src, runtime, memory, language=""):

    if(language == "java"):
        print("gcloud functions deploy " + funcName + " --source " + src + " --runtime " + runtime + " --memory " + memory + " --trigger-http --allow-unauthenticated --entry-point functions." + funcName)
        os.system("gcloud functions deploy " + funcName + " --source " + src + " --runtime " + runtime + " --memory " + memory + " --trigger-http --allow-unauthenticated --entry-point functions." + funcName)
    else:
        print("gcloud functions deploy " + funcName + " --source " + src + " --runtime " + runtime + " --memory " + memory + " --trigger-http --allow-unauthenticated")
        os.system("gcloud functions deploy " + funcName + " --source " + src + " --runtime " + runtime + " --memory " + memory + " --trigger-http --allow-unauthenticated")

def getFunctionLogs(funcName, limit, since, outputFile):
    os.system('gcloud logging read "resource.type=cloud_function AND resource.labels.function_name=' + funcName + ' AND resource.labels.region=us-central1 AND severity>=DEFAULT" --format=json --limit=' + limit + ' > ' + outputFile)