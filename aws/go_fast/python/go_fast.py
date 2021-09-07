import numpy as np
import numba as nb
import time
import os
import multiprocessing

nb.config.THREADING_LAYER = 'tbb'

@nb.jit(nopython=True, parallel=True, fastmath=True)
def go_fast_parallel(a):
    trace = 0.0
    for i in range(a.shape[0]):
        trace += np.tanh(a[i, i])
    return a + trace

@nb.jit(nopython=True, parallel=False, fastmath=True)
def go_fast_sequential(a):
    trace = 0.0
    for i in range(a.shape[0]):
        trace += np.tanh(a[i, i])
    return a + trace


def initialize(N):
    from numpy.random import default_rng
    rng = default_rng(42)
    x = rng.random((N, N), dtype=np.float64)
    return x

invocationCount = 0

def run_Go_fast(input, threads):
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
        N = 6000
    elif(input == "L"):
        N = 22000

    cpus = multiprocessing.cpu_count()
    if(threads > cpus):
        threads = cpus

    x = initialize(N)
    t = time.time()
    if(threads > 1):
        go_fast_parallel(x)
    else:
        go_fast_sequential(x)
    t = time.time() - t
    return "Execution Time: " + str(t * 1000) + "ms, Threads: " + str(threads) + "\n" + addInfo



def lambda_handler(event, context):
    print(event)
    msg = run_Go_fast(event['Input'], event['Threads'])
    return { 'message': msg }