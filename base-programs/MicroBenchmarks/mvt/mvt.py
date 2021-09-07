import numpy as np
import numba as nb

nb.jit(nopython=True, parallel=True, fastmath=True)


def kernel(x1, x2, y_1, y_2, A):

    x1 += A @ y_1
    x2 += y_2 @ A



import numpy as np


def initialize(N, datatype=np.float64):
    x1 = np.fromfunction(lambda i: (i % N) / N, (N, ), dtype=datatype)
    x2 = np.fromfunction(lambda i: ((i + 1) % N) / N, (N, ), dtype=datatype)
    y_1 = np.fromfunction(lambda i: ((i + 3) % N) / N, (N, ), dtype=datatype)
    y_2 = np.fromfunction(lambda i: ((i + 4) % N) / N, (N, ), dtype=datatype)
    A = np.fromfunction(lambda i, j: (i * j % N) / N, (N, N), dtype=datatype)

    return x1, x2, y_1, y_2, A
