#
import argparse
import numpy as np
import re
from dhnnlib import *
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
#
description = """
    Generate in/T-alphaHP/<input>.txt files according to desiderd distribution 
    or specific values of alpha, starting from (N, alpha, Th) values in parsed input
    file.
"""
hPars_fn = """
    a string for the input file name to be used to generate input files
"""
hPars_p = """
    (int) the value p of the p--body interaction type
"""
hPars_Tm = """
    [OPTIONAL] (float) T starting value \t|default=0.01
"""
hPars_TM = """
    [OPTIONAL] (float) T starting value \t|default=3.33
"""
hPars_TPts = """
    [OPTIONAL] (int) number of points along T for modes LIN/EXS\n
    \n\t|default=8
"""
hPars_mode = """
    [OPTIONAL] (str) mode for generating alpha sampling\n
    possible modes are:\n
        -LIN: for linear spacing across alpha\n\n
        -EXS: for a denser sampling on extremal values (T~0, T~∞)\n\n
        -CLV: for sampling around the critical line
    \n\t|default=LIN
"""
#
parser = argparse.ArgumentParser(description=description)
parser.add_argument('fn',
                    type=str,
                    help=hPars_fn
                    )
parser.add_argument('-p', '--pbody',
                    type=int,
                    default=2,
                    help=hPars_p
                    )
parser.add_argument('-m', '--mode',
                    type=str,
                    default="LIN",
                    help=hPars_mode
                    )
parser.add_argument('--Tm',
                    type=float,
                    default=1e-2,
                    help=hPars_Tm
                    )
parser.add_argument('--TM',
                    type=float,
                    default=1.60e-1,
                    help=hPars_TM
                    )
parser.add_argument('--TPts',
                    type=int,
                    default=8,
                    help=hPars_TPts
                    )
args = parser.parse_args()
#
dtInF = [('N', np.uint32), ('α', np.double), ('Th', np.double)]
p = args.pbody
Tm = args.Tm
TM = args.TM
TPts = args.TPts
inF = np.loadtxt(args.fn, dtype=dtInF)
if args.mode == "LIN":
    TLs = np.linspace(Tm, TM, num=TPts, endpoint=False)
if args.mode == "EXS":
    numLow = int(2*TPts/5)
    numHigh = int(2*TPts/5)
    numMid = int(TPts/5)
    TLs = np.linspace(Tm, TM/3, num=numLow, endpoint=False)
    TLs = np.hstack(TLs, np.linspace(TM/3, TM*2./3, num=numMid, endpoint=False))
    TLs = np.hstack(TLs, np.linspace(TM*2./3, TM, num=numHigh, endpoint=False))
if args.mode == "EXS" or args.mode == "LIN":
    #
    for vs in inF:
        strSave = f"in/alpha{p:d}-T/N={vs['N']:d}_T={vs['T']:g}.txt"
        np.savetxt(strSave, TLs, delimiter="\n", fmt="%.3g")
elif args.mode == "CLV":
    for vs in inF:
        if not (vs['α'] < αcStiefvater):
            continue
        Tc = 1./βcH2(vs['α'])
        TLs = np.hstack([Tc, 1.05*Tc, 1.1*Tc, 1.5*Tc, 2*Tc])
        if all(pFlip(1./T)< 1e-2 for T in TLs):
            continue
        strSave = f"in/T-alpha{p:d}/N={vs['N']:d}_K={int(vs['α']*vs['N']):d}.txt"
        np.savetxt(strSave, TLs, delimiter="\n", fmt="%.3g")