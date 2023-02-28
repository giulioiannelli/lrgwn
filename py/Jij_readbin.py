import numpy as np
#
def readJ(N: np.uint32, fname: str) -> np.array:
    r"""Read J_ij matrices from binary files.

    The coupling matrices are stored as only the lower triangular part, without
    diagonal, being J_{ii} = 0 and J_{ij} = J_{ji}. They are saved as int32 
    numbers.
    We thus read the numbers, creare a list of lists whose entries are the
    lower triangular matrix rows and then flatten for passing to numpy function
    `np.triu_indices`.

    Parameters
    ----------
    N : int
        System size.
    fname : str
        Name of the binary file to read the matrices from.
    """
    triu, tril = np.triu_indices(N), np.tril_indices(N, -1)
    J = np.zeros((N, N))
    Jbin = np.fromfile(fname, dtype=np.int32)
    Jlt = [[Jbin[((i*(i-1))//2+j)] for j in range(i)] for i in range(N)]
    JltF = np.array([i for sbl in Jlt for i in sbl])
    J[tril] = JltF
    J[triu] = J.T[triu]
    return J