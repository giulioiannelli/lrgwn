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
#
#
description = """
    
"""
hPars_inf = """
    (str) name of the file to read the input from
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
prsr.add_argument('infname',
                  type=str,
                  help=hPars_inf
                  )
prsr.add_argument('nAvg',
                  type=int,
                  help=hPars_nA
                  )
prsr.add_argument('--pname',
                  type=str,
                  default="Jxixi_",
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
fname = args.infname
nAvg = args.nAvg
pname = args.pname
saveJ = args.saveJ
#
nAvgStr = "nAvg"
TStr = "T"
KStr = "K"
#
resdat = "res/data/"
resdtj = f"{resdat}jmat2/"
inset = np.loadtxt(fname)
#
for vs in inset:
    N = int(vs[0])
    T = vs[1]
    K = int(vs[2])
    beta = 1./T
    if (pFlip(beta) < 1e-3):
        continue
    resdtN = f"{resdtj}N={N:d}/"
    makedirs(resdtN, exist_ok=True)
    fnameJ = f"{resdtN}T={T:.3g}_K={K:d}.bin"
    callJxixi = [f"exe/{pname}.o", f"{N:d}", f"{T:.3g}", f"{K:d}"]
    subprocess.call(callJxixi)
