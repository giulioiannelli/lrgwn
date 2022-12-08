import argparse
import numpy as np
import subprocess
#
from os import remove
from os.path import exists
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import eigs
#
#
# betamin, betamax = 1e-4, 3.33
# betalist = np.linspace(betamin, betamax, num=4)
# alphmin, alphmax = 5/N, 0.35
# alphlist = np.linspace(alphmin, alphmax, num=4)
betals = [0.5, 1, 2]
alphls = [0.01, 0.1, 0.2]
#
#
def pFlip(beta):
    r"""Compute the flip probability of the Markov Process generating patterns.
    
    This function compute the flip probability of the two-body correlated 
    Markov process which is used to create proxy patters of the 1D
    Ising model. When `beta=0` (i.e. no correlation) `pFlip=0.5`.
    We use this to avoid computing the matrices when the probability of getting 
    "frozen" configurations is non negligible. Indeed, if `pFlip` is too 
    small, i.e. `beta` high, patterns eventually become stringd of \pm 1s.
    
    Parameters
    ----------
    beta : float
        The inverse temperature of 1D Ising model setting the correlation level 
        in the Markov process generation.
    """
    return (1 - .5 * (np.tanh(beta) + 1))
def readJ(N, fname):
    r"""Read J_ij matrices from binary file generated through C program Jxixi.

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
    JltF = np.array([item for sublist in Jlt for item in sublist])
    J[tril] = JltF
    J[triu] = J.T[triu]
    return J
def makeSME(J):
    r"""Compute the smallest eigenvalue of the Laplacian of J. 
    
    Parameters
    ----------
    J : array
        The coupling matrix of which one wants to compute the eigenvalue.
    """
    A = np.copy(J)
    D = abs(A).sum(axis=1)
    L = np.diag(D)-A
    spA = csr_matrix(L)
    eig = eigs(spA, k=1, which='SM')[0].item()
    return np.real(eig)
#
#
description = """
    ...
"""
hPars_N = """
    (int) system size
"""
default_pn = "| default=genmat"
hPars_pn = f"""
    (str) C program name {default_pn:->30}
"""
default_sJ = "| default=genmat"
hPars_sJ = f"""
    (bool) store J matrix in a file in binary form {default_sJ:->30}
"""
prsr = argparse.ArgumentParser(description=description)
prsr.add_argument('N',
                  type=int,
                  help=hPars_N
                  )
prsr.add_argument('--pname',
                  type=str,
                  default="genmat",
                  help=hPars_pn
                  )
prsr.add_argument('--saveJ',
                  action='store_true',
                  help=hPars_sJ
                  )
prsr.add_argument('--no-saveJ',
                  dest='feature',
                  action='store_false'
                  )
prsr.set_defaults(feature=True)
args = prsr.parse_args()
N = args.N
saveJ = args.saveJ
#
#
gccComp = f"gcc -O3 -DSFMT_MEXP=19937 -o exe/{args.pname}.o "\
    "C/head/SFMT/SFMT.c C/Jxixi.c -Wall -lm"
subprocess.run(gccComp.split(' '))
#
lsEig = []
for beta in betals:
    if (pFlip(beta) < 1e-3):
        continue
    T = 1./beta
    for alph in alphls:
        K = int(alph*N)
        fnameE = f"res/tmp/eigs_tmp_N={N:d}_T={T:.3g}_K={K:d}.dat"
        if exists(fnameE):
            eig  = np.loadtxt(fnameE)[2]
            lsEig.append([T, K, eig])
            continue
        fnameJ = f"res/data/jmat2/N={N:d}_T={T:.3g}_K={K:d}.bin"
        subprocess.call([f"exe/{args.pname}.o", f"{N:d}", f"{T:.3g}", f"{K:d}"])
        J = readJ(N, fnameJ)/N
        eig = makeSME(J)
        lsEig.append([T, K, eig])
        np.savetxt(fnameE, [np.array([T, K, eig])], fmt='%.3g\t%d\t%g')
        if not saveJ:
            remove(fnameJ)
fnameE = f"res/data/phdg/eigs_{args.pname}_N={N:d}.dat"
np.savetxt(fnameE, np.array(lsEig), fmt='%.3g\t%d\t%g')