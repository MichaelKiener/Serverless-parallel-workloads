import numpy as np
import numba as nb
import sys
import time
import os
import multiprocessing



@nb.jit(nopython=True, parallel=False, fastmath=True)
def kernel_sequential(A, x):
    return (A @ x) @ A

@nb.jit(nopython=True, parallel=True, fastmath=True)
def kernel_parallel(A, x):
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
    if(threads > 1):
        kernel_sequential(A,x)
    else:
        kernel_parallel(A,x)
    t = time.time() - t
    return "Execution Time: " + str(t * 1000) + "ms, Threads: " + str(threads)
