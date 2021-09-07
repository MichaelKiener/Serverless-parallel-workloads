import numpy as np


def initialize(N):
    from numpy.random import default_rng
    rng = default_rng(42)
    x = rng.random((N, N), dtype=np.float64)
    return x



def go_fast(a):
    trace = 0.0
    for i in range(a.shape[0]):
        trace += np.tanh(a[i, i])
    return a + trace
