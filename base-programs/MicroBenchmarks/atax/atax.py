import numpy as np
import numba as nb


@nb.jit(nopython=True, parallel=False, fastmath=True)
def kernel(A, x):

    return (A @ x) @ A

def initialize(M, N, datatype=np.float64):
    fn = datatype(N)
    x = np.fromfunction(lambda i: 1 + (i / fn), (N, ), dtype=datatype)
    A = np.fromfunction(lambda i, j: ((i + j) % N) / (5 * M), (M, N),
                        dtype=datatype)

    return x, A
