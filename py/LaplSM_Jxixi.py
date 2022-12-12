import argparse
import numpy as np
import subprocess
#
from os import remove, mkdir, makedirs, stat
from os.path import exists
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import eigs
#
#
def splash(*args):
    pass

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
    Compute the smallest eigenvalue associated to the Laplacian of coupling 
    matrices. The coupling matrices are obtained by outer product of "patterns"
    exhibiting spatial correlation. The correlations are tuned via the 
    "temperature parameter" of a Markov process with two point correlations 
    exponential in temperature and distance. 
"""
hPars_N = """
    (int) system size
"""
hPars_b = """
    (float) inverse temperature of the generating patterns beta = 1/T 
"""
hPars_a = """
    (float) storage load of the network alph = K/N
"""
hPars_nA = """
    (int) number of averages
"""
default_pn = "| default=genmat"
hPars_pn = f"""
    (str) C program name {default_pn:->10}
"""
default_sJ = "| default=--no-saveJ"
hPars_sJ = f"""
    (bool) store J matrix in a file in binary form {default_sJ:->10}
"""
prsr = argparse.ArgumentParser(description=description)
prsr.add_argument('N',
                  type=int,
                  help=hPars_N
                  )
prsr.add_argument('beta',
                  type=float,
                  help=hPars_b
                  )
prsr.add_argument('alph',
                  type=float,
                  help=hPars_a
                  )
prsr.add_argument('nAvg',
                  type=int,
                  help=hPars_nA
                  )
prsr.add_argument('--pname',
                  type=str,
                  default="Jxixi",
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
beta = args.beta
alph = args.alph
T = 1./beta
K = int(alph*N)
nAvg = args.nAvg
pname = args.pname
saveJ = args.saveJ
#
nAvgStr = "nAvg"
TStr = "T"
KStr = "K"
sumEStr = "sumE"
sumEsqStr = "sumEsq"
fmtStr = [nAvgStr, TStr, KStr, sumEStr, sumEsqStr]
fmtLen = max([len(fS) for fS in fmtStr])
# hdrTxt = [f"{fS:<{15}}" for fS in fmtStr]
# header = " ".join(hdrTxt)
fmt = " ".join([f"%-{fmtLen:d}d", f"%-{fmtLen:d}.3e", f"%-{fmtLen:d}d",
                f"%-{fmtLen:d}.7e", f"%-{fmtLen:d}.7e"])
#
restmp = "res/tmp/"
restmN = f"{restmp}N={N:d}/"
resdat = "res/data/"
resdtj = f"{resdat}jmat2/"
resphg = f"{resdat}phdg/"
fnameE = f"{restmN}/eigs_T={T:.3g}_K={K:d}.dat"
fnameJ = f"{resdtj}N={N:d}_T={T:.3g}_K={K:d}.bin"
callJxixi = [f"exe/{pname}.o", f"{N:d}", f"{T:.3g}", f"{K:d}"]
#
makedirs(restmp, exist_ok=True)
makedirs(restmN, exist_ok=True)
makedirs(resphg, exist_ok=True)
#
if (pFlip(beta) < 1e-3):
    exit()
#
lsEig = []
lsEigsq = []
doneAvg = 0
missAvg = nAvg
if exists(fnameE) and stat(fnameE).st_size:
    eig_vals = np.loadtxt(fnameE)
    missAvg = int(nAvg-eig_vals[0])
    doneAvg = int(eig_vals[0])
if not missAvg:
    exit()
if nAvg-missAvg:
    lsEig.append(eig_vals[3]*nAvg)
    lsEigsq.append(eig_vals[4]*nAvg)
if not saveJ:
    makeJ = remove
else:
    makeJ = splash
for i in range(doneAvg, nAvg):
    subprocess.call(callJxixi)
    J = readJ(N, fnameJ)/N
    eig = makeSME(J)
    lsEig.append(eig)
    lsEigsq.append(eig*eig)
    saveout = np.array([i+1, T, K, sum(lsEig)/nAvg, sum(lsEigsq)/nAvg])
    np.savetxt(fnameE, saveout[None], fmt=fmt)
    makeJ(fnameJ)
