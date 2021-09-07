import numpy as np
import numba as nb
import time
import os
import multiprocessing

nb.config.THREADING_LAYER = 'tbb'

nb.jit(nopython=True, parallel=True, fastmath=True)
def kernel_parallel(x1, x2, y_1, y_2, A):

    x1 += A @ y_1
    x2 += y_2 @ A


nb.jit(nopython=True, parallel=False, fastmath=True)
def kernel_sequential(x1, x2, y_1, y_2, A):

    x1 += A @ y_1
    x2 += y_2 @ A

def initialize(N, datatype=np.float64):
    x1 = np.fromfunction(lambda i: (i % N) / N, (N, ), dtype=datatype)
    x2 = np.fromfunction(lambda i: ((i + 1) % N) / N, (N, ), dtype=datatype)
    y_1 = np.fromfunction(lambda i: ((i + 3) % N) / N, (N, ), dtype=datatype)
    y_2 = np.fromfunction(lambda i: ((i + 4) % N) / N, (N, ), dtype=datatype)
    A = np.fromfunction(lambda i, j: (i * j % N) / N, (N, N), dtype=datatype)

    return x1, x2, y_1, y_2, A

invocationCount = 0

def run_MVT(input, threads):
    N = 0
    global invocationCount
    invocationCount += 1
    addInfo = str(invocationCount)
    if(invocationCount <= 1):
        f = open("/proc/cpuinfo","r")
        for line in f.readlines():
            addInfo += line
    if (input == "S"):
        N = 2000
    elif (input == "M"):
        N = 5500
    elif(input == "L"):
        N = 22000

    x1, x2, y_1, y_2, A = initialize(N)
    t = time.time()
    if(threads > 1):
        kernel_parallel(x1, x2, y_1, y_2, A)
        threads = os.cpu_count()
    else:
        kernel_sequential(x1, x2, y_1, y_2, A)
        threads = 1
    t = time.time() - t
    return "Execution Time: " + str(t * 1000) + "ms, Threads: " + str(threads) + "\n" + addInfo



def MVT_py(request):
    request_json = request.get_json(silent=True)
    threads = 0
    input = "S"
    if request_json and 'Threads' in request_json:
        threads = request_json['Threads']
        input = request_json['Input']

    executionID = request.headers.get('function-execution-id')
    msg = run_MVT(input, threads)
    msg += "\n ExecutionID: " + executionID
    return { 'message': msg }