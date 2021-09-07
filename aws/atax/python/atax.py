import numpy as np
import numba as nb
import time
import multiprocessing



@nb.jit(nopython=True, parallel=False, fastmath=True)
def kernel_sequential(A, x):
    return (A @ x) @ A

def initialize(M, N, datatype=np.float64):
    fn = datatype(N)
    x = np.fromfunction(lambda i: 1 + (i / fn), (N, ), dtype=datatype)
    A = np.fromfunction(lambda i, j: ((i + j) % N) / (5 * M), (M, N),
                        dtype=datatype)

    return x, A

invocationCount = 0

def run_Atax(input, threads):
    N = 0
    M = 0
    global invocationCount
    invocationCount += 1
    addInfo = str(invocationCount)
    if(invocationCount <= 1):
        f = open("/proc/cpuinfo","r")
        for line in f.readlines():
            addInfo += line

    if (input == "S"):
        M = 2000
        N = 2500
    elif (input == "M"):
        M = 4000
        N = 5000
    elif(input == "L"):
        M = 20000
        N = 22000

    cpus = multiprocessing.cpu_count()
    if(threads > cpus):
        threads = cpus

    x, A = initialize(M, N)
    t = time.time()
    kernel_sequential(A,x)
    t = time.time() - t
    return "Execution Time: " + str(t * 1000) + "ms, Threads: " + str(threads) + "\n" + addInfo



def lambda_handler(event, context):
    print(event)
    msg = run_Atax(event['Input'], event['Threads'])
    return { 'message': msg }