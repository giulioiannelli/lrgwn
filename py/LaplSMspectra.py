import argparse
import numpy as np
import subprocess

from os.path import exists
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import eigs
#
#
def pFlip(beta):
    return (1 - (np.tanh(beta) + 1)/2)
def readJmat(fname):
    Jbin = np.fromfile(fname, dtype=np.int32)
    Jltm = [[Jbin[((i*(i-1))//2+j)] for j in range(i)] for i in range(N)]
    Jltm = np.array([item for sublist in Jltm for item in sublist])
    J = np.zeros((N, N))
    triu, tril = np.triu_indices(N), np.tril_indices(N, -1)
    J[tril] = Jltm
    J[triu] = J.T[triu]
    return J
def makeEig(J):
    A = np.copy(J)
    D = abs(A).sum(axis=1)
    L = np.diag(D)-A
    spA = csr_matrix(L)
    eig = eigs(spA, k=1, which='SM')[0].item()
    return np.real(eig)
#
description = """
    ...
"""
hPars_N = """
    (int) system size
"""
hPars_pn = """
    [OPTIONAL] (str) C program name \t|default=genmat
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
args = prsr.parse_args()
N = args.N
gccComp = f"gcc -O3 -DSFMT_MEXP=19937 -o exe/{args.pname}.o "\
    "C/head/SFMT/SFMT.c C/Jxixi.c -Wall -lm"
subprocess.run(gccComp.split(' '))
#

fnameE = f"res/tmp/eigs_tmp_N={N:d}.dat"
# qui calcola la temperatura alla quale ho stati frozen = betamax
# calcola anche betamin dopo il quale e come N
betamin, betamax = 1e-4, 3.33
betalist = np.linspace(betamin, betamax, num=4)
alphmin, alphmax = 5/N, 0.35
alphlist = np.linspace(alphmin, alphmax, num=4)
lsEig = []
for beta in betalist:
    if (pFlip(beta) * N < 5):
        continue
        #skip cause not enough flips
    for alph in alphlist:
        T = 1./beta
        K = int(alph*N)
        fnameJ = f"res/tmp/N={N:d}_T={T:.3g}_K={K:d}.bin"
        subprocess.call([f"exe/{args.pname}.o", f"{N:d}", f"{T:.3g}", f"{K:d}"])        
        J = readJmat(fnameJ)
        eig = makeEig(J)
        lsEig.append([T, K, eig])
        np.savetxt(fnameE, np.array(lsEig), fmt='%.3g\t%d\t%g')