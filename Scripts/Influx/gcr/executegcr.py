import requests
import sys
from datetime import datetime
from requests.structures import CaseInsensitiveDict
import os
import time
import json
import uuid

def callFunction(funcName, input, threads, memory):
    # os.system("gcloud -v")
    print("Calling: " + funcName + " with input " + input + " and " + str(threads) + " threads")
    callID = str(uuid.uuid1())
    urlNames = json.load(open('./caasConfig.json', 'r'))
    url = urlNames[funcName]["url"]
    url += "/" + callID
    headers = CaseInsensitiveDict()
    headers["Content-Type"] = "application/json"
    data = {
        "Input": input,
        "Threads": threads
    }
    r = requests.post(url, data=json.dumps(data, indent=2), headers=headers)
    print('Status: ', r.status_code)
    print('Response:', r.content)
    res = r.content + "\n CallingID: " + callID
    return res

def updateFunction(funcName, src, runtime, memory, language=""):
    config = json.load(open('./caasConfig.json', 'r'))
    service = config[funcName]['serviceName']
    cpus = 2
    if(memory == '2048'):
        cpus = 4
        memory = '2148'
    if(memory == '4096'):
        cpus = 4
    if(memory == '8192'):
        cpus = 4
    funcName = funcName.replace("_", "-")
    funcName = funcName.lower()
    print("gcloud run services update " + funcName + " --memory " + memory + "M --cpu " + str(cpus) + " --region us-central1")
    os.system("gcloud run services update " + funcName + " --memory " + memory + "M --cpu " + str(cpus) + " --region us-central1")

def getFunctionLogs(funcName, limit, since, outputFile):
    config = json.load(open('./caasConfig.json', 'r'))
    service = config[funcName]['serviceName']
    # service = funcName.replace('_', '')
    os.system('gcloud logging read "resource.type=cloud_run_revision AND resource.labels.service_name=' + service + '" --project ma-stateful-serverless --format=json --limit=' + limit + ' > ' + outputFile)