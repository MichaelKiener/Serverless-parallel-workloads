import requests
import sys
from datetime import datetime
import os
import time
import json


if(sys.argv[1] == 'aws'):
    sys.path.insert(1, './aws')
    import executeAWS as updater
elif (sys.argv[1] == 'google'):
    sys.path.insert(1, './google')
    import executegcf as updater
elif (sys.argv[1] == 'gcr'):
    sys.path.insert(1, './gcr')
    import executegcr as updater
elif (sys.argv[1] == 'ow'):
    sys.path.insert(1, './ow')
    import executeOW as updater

funcDependencies = json.load(open('./functionNames.json', 'r'))
functionName = funcDependencies[sys.argv[2]][sys.argv[3]]

funcSrc = json.load(open('./funcSrc.json', 'r'))
src = funcSrc[sys.argv[1]][sys.argv[2]][sys.argv[3]]

funcRuntime = json.load(open('./runtimes.json', 'r'))
run = funcRuntime[sys.argv[1]][sys.argv[3]]

updater.updateFunction(functionName, src, run, sys.argv[4], sys.argv[3])



