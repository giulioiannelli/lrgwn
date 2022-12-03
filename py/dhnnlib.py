#
# general functions, independent from imports
def rlen(ls):
    return range(len(ls))
def shuffled(ls):
    shuffle(ls)
    return ls
def dictcp(dc):
    return {k: dc[k] for k in dc}
#
#
from cycler import cycler
from matplotlib import cm, rc
from matplotlib.colors import ListedColormap
from matplotlib.legend_handler import HandlerTuple
from matplotlib.lines import Line2D
from mpl_toolkits.axes_grid1.inset_locator import inset_axes, mark_inset
from numpy import abs, arctanh, argsort, array, copy, diag, fromfile,\
                  genfromtxt, isreal, loadtxt, mean, real, sqrt, std,\
                  tril_indices, triu_indices, unique, var, where, zeros
from os import listdir, chdir, makedirs
from os.path import exists
from random import shuffle
from re import findall
from scipy.interpolate import griddata as gd
from scipy.signal import savgol_filter
from scipy.optimize import curve_fit, root, fsolve
from scipy.sparse.linalg import eigs
from scipy.sparse import csr_matrix
#
# matplotlib aesthetics
def setCmap(cmName='cmap', cmapChoose='summer'):
    exec(f"{cmName} = cm.{cmapChoose}")
    return locals()['cmap']
unw = {None, 'None', '', ' ', '.', ',', '1', '3', '2', '4', '^', '<', 'v', '|',
       '_', 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 'x', '+'}
list_of_markers = [mkrs for mkrs in [*Line2D.markers.keys()] if mkrs not in unw]
#
# HNN numbers & functions
αcCrisanti = 0.144
αcStiefvater = 0.141
αcAmit = 0.1382
αcAbbott = 0.135
mSompolinsky = 0.967
mAbbott = 0.872

def βcH2(α, αc=αcStiefvater):
    argatan = (2*(αc-α)/(α**2-2*α*αc+2*αc+α*(α**2+4*(1-αc))**(.5)))**(.5)
    return arctanh(argatan)

def TcH2(α, αc=αcStiefvater):
    return 1./βcH2(α, αc=αc)

def βcH3(α, αc=αcAbbott):
    argatan = sqrt((αc-α)/(αc+sqrt(2*(αc*α+α*α))+α))
    return arctanh(argatan)

def TcH3(α, αc=αcAbbott):
    return 1./βcH3(α, αc=αc)

def TcH2root(α, t=0, αc=αcStiefvater):
    return 1/βcH2(α, αc=αc) - t

dtN = ('N', 'u4')
dtK = ('K', 'u4')
dtJ = ('J', 'i4')
dtT = ('T', 'f8')
dtα = ('α', 'f8')
dtξ = ('ξ', 'i1')
dtTh = ('Th', 'f8')
dtIn = [dtN, dtTh]
dtKTi = [dtK, dtT]
dtInα = [dtN, dtα, dtTh]
dtInT = [dtN, dtT, dtTh]

etxt = ".txt"
edat = ".dat"
ebin = ".bin"
epdf = ".pdf"
epng = ".png"

ISPARENT = ".isparent"

reTK = r"\b=(\d+(?:\.\d+)?(?:[eE][-+]?[0-9]+)?)"

class dhnnlib:
    
    # -------------------------------------------------------------------------
    #
    class MAKE_DIRTREE:
        #
        def ifDBACKelse(self, back, DIR):
            return "../" if back else "" + DIR
        #
        def __init__(self, Hp, DATAF=None):
            back = not exists(".isparent")
            # level 1
            self.DMAIN = self.ifDBACKelse(back, "./")
            self.DCPRO = self.ifDBACKelse(back, "C/")
            self.DEXEC = self.ifDBACKelse(back, "exe/")
            self.DINPT = self.ifDBACKelse(back, "in/")
            self.DPYPR = self.ifDBACKelse(back, "py/")
            self.DLOGF = self.ifDBACKelse(back, "log/")
            self.DRESF = self.ifDBACKelse(back, "res/")
            # level 2
            self.DDATA = self.DRESF + "data/"
            self.DPLOT = self.DRESF + "plot/"
            self.DUNTC = self.DRESF + "untch/"
            self.D4PLT = self.DRESF + "4plt/"
            self.DDATF = DATAF if DATAF else self.DDATA
            # level3
            self.DHOPF = self.DDATF + f"hop{Hp:d}/"
            self.DJMAT = self.DDATA + f"jmat{Hp:d}/"
            self.DHEAD = self.DCPRO + "head/"
            # level 4
            self.DFCFG = self.DDATA + "fcfg/"
            self.DCHCK = self.DDATA + "chck/"
            self.DCFGS = self.DDATA + "cfgs/"
            self.DHOP2 = self.DDATA + "hop2/"
            self.DHOP3 = self.DDATA + "hop3/"
            self.DJMA2 = self.DDATA + "jmat2/"
            self.DJMA3 = self.DDATA + "jmat3/"
            self.DSFMT = self.DHEAD + "SFMT/"
    #
    # -------------------------------------------------------------------------
    #
    def __init__(self, infn, Hp, DATAF=None, thMOD='MIDPOINT', ptyp="ISING",
                 avgm=1, AVGD=False, mS=mSompolinsky, mA=mAbbott):
        self.Hp, self.infn, self.ptyp, self.avgm = Hp, infn, ptyp, avgm
        self.mS, self.mA = mSompolinsky, mA
        self.thMOD = thMOD
        self.dTree = self.MAKE_DIRTREE(Hp=Hp, DATAF=DATAF)
        if genfromtxt(self.infn).size == 2:
            Ntmp = genfromtxt(self.infn, dtype=dtIn)['N'].item()
            Thtmp = genfromtxt(self.infn, dtype=dtIn)['Th'].item()
            self.din = [{'N': Ntmp, 'Th': Thtmp}]
            self.AVGD = {Ntmp: AVGD}
        else:
            self.din = [{'N': inv[0], 'Th':inv[1]}
                           for inv in genfromtxt(self.infn, dtype=dtIn)]
            self.AVGD = {N: AVGD[N] if hasattr(AVGD, "__len__") else AVGD 
                         for N in self.lsN}
        self.lsN = unique([d['N'] for d in self.din])
        if Hp == 2:
            self.Tcfit = TcH2
            self.βcfit = βcH2
        elif Hp == 3:
            self.Tcfit = TcH3
            self.βcfit = βcH3
    #
    # -------------------------------------------------------------------------
    #
    def GET__vXk(self, d, key):
        return [dd[key] for dd in d]

    def GET__dXvk(self, d, key, val):
        return [dd for dd in d if dd[key] == val]

    def GET__dSXk(self, d, key):
        return [d[kvs] for kvs in argsort(self.GET__vXk(d, key))]

    def GET__UvXk(self, d, key):
        return list(set(self.GET__vXk(d, key)))

    def GET__dETh(self):
        return [{'N': N, 'Th': self.MAKE__ThXN(N)} 
                for N in self.GET__UvXk(self.din, 'N')]
    
    def GET__strJXKTi(self, N, K, Ti, avg=0):
        pdir = self.dTree.DJMAT
        return f"{pdir}{self.ptyp}_N={N:d}/T={Ti:g}_K={K:d}_{avg:d}{ebin}"

    #
    # -------------------------------------------------------------------------
    #
    def MAKE__dN(self, N):
        return [dictcp(d) for d in self.GET__dETh() if d['N'] == N]

    def MAKE__Jmat(self, N, K, Ti, avg=0):
        Jbin = fromfile(self.GET__strJXKTi(N, K, Ti, avg=avg), dtype=[dtJ])['J']
        Jltm = [[Jbin[((i*(i-1))//2+j)] for j in range(i)] for i in range(N)]
        Jltm = array([item for sublist in Jltm for item in sublist])
        J = zeros((N, N))
        triu, tril = triu_indices(N), tril_indices(N, -1)
        J[tril] = Jltm 
        J[triu] = J.T[triu]
        return J
    
    def MAKE__JLeigSM(self, J):
        A = copy(J)
        D = abs(A).sum(axis=1)
        L = diag(D)-A
        spA = csr_matrix(L)
        eig = eigs(spA, k=1, which='SM')[0].item()
        if not isreal(eig):
            print("FATAL ERROR")
        return real(eig)

    def MAKE__ThXN(self, N):
        ld = listdir(self.dTree.DHOPF +\
            f"{self.ptyp}_N={N:d}_avg={self.avgm:d}/")
        return [float(findall(reTK, dat)[0]) for dat in ld]

    def MAKE__KTiXN_J(self, N):
        ld = listdir(self.dTree.DJMAT + f"{self.ptyp}_N={N:d}/")
        return {'N': N, 'TiK': [{'Ti': float(findall(reTK, dat)[0]),
                'K': int(findall(reTK, dat)[1])} for dat in ld]}

    def MAKE__KTi_X_NTh(self, N, Th):
        ld = listdir(self.dTree.DHOPF +
                     f"{self.ptyp}_N={N:d}_avg={self.avgm:d}/Th={Th:g}/")
        return {'N': N, 'Th': Th, 'TiK': [{'Ti': float(findall(reTK, dat)[0]),
                'K': int(findall(reTK, dat)[1])} for dat in ld]}
    
    def MAKE__alphaTi_X_NTh(self, N, Th):
        ld = listdir(self.dTree.DHOPF +
                     f"{self.ptyp}_N={N:d}_avg={self.avgm:d}/Th={Th:g}/")
        return {'N': N, 'Th': Th, 'TiK': [{'Ti': float(findall(reTK, dat)[1]),
                'K': float(findall(reTK, dat)[0])*N**(self.Hp-1)} for dat in ld]}

    def MAKE__dkc(self, N, th, K, dkc):
        dkc = {'K': K, 'Ti': sorted([d['Ti'] for d in dkc])}
        def strdat(Ti): 
            parentFldr = f"{self.ptyp}_N={N:d}_avg={self.avgm:d}/"
            return f"{parentFldr}Th={th:g}/T={Ti:g}_K={dkc['K']:d}"
        if self.AVGD[N]:
            def mmattis(Ti):
                ldtxt = loadtxt(self.dTree.DHOPF+strdat(Ti)+edat)
                return abs(ldtxt)
        else:
            def mmattis(Ti):
                pth = self.dTree.DHOPF+strdat(Ti)
                ldtxt = array([abs(loadtxt(pth+f"_{m:d}"+edat))
                                for m in range(self.avgm)])
                return ldtxt.mean(axis=0)
        m_avg = [mean(mmattis(Ti)) for Ti in dkc['Ti']]
        m_var = [std(mmattis(Ti)) for Ti in dkc['Ti']]

        dkc['<m>'] = m_avg
        dkc['VARm'] = m_var
        return dkc

    def MAKE__dkc_nom(self, K, dkc):
        dkc = {'K': K, 'Ti': sorted([d['Ti'] for d in dkc])}
        return dkc

    def MAKE__dkc_T_i(self, N, th, Ti, dkc):
        dkc = {'Ti': Ti, 'K': sorted([d['K'] for d in dkc])}
        if self.AVGD[N]:
            def strdat(K): 
                parentFldr = f"{self.ptyp}_N={N:d}_avg={self.avgm:d}/"
                return f"{parentFldr}Th={th:g}/T={dkc['Ti']:.2g}_K={K:d}"
            def mmattis(K):
                ldtxt = loadtxt(self.dTree.DHOPF+strdat(K)+edat)
                return abs(ldtxt)
        else:
            def strdat(K): 
                parentFldr = f"{self.ptyp}_N={N:d}_avg={self.avgm:d}/"
                return f"{parentFldr}Th={th:g}/alpha={K/N**(self.Hp-1):.3g}_T={dkc['Ti']:.2g}"
            def mmattis(K):
                pth = self.dTree.DHOPF+strdat(K)
                ldtxt = array([abs(loadtxt(pth+f"_{m:d}"+edat))
                                for m in range(self.avgm)])
                return ldtxt.mean(axis=0)
        m_avg = [mean(mmattis(K)) for K in dkc['K']]
        m_var = [std(mmattis(K)) for K in dkc['K']]

        dkc['<m>'] = m_avg
        dkc['VARm'] = m_var
        return dkc

    def MAKE__critIsT(self, d, K):
        dkc = self.MAKE__dkc(d['N'], d['Th'], K,
                             self.GET__dXvk(d['TiK'], 'K', K))
        if self.Hp == 2:
            mThresh = self.mS
        elif self.Hp == 3:
            mThresh = self.mA
        if self.thMOD == 'SAVGOL':
            savgol_filtd = savgol_filter(array(dkc['<m>']), 3, 5)
            thCon = where(array(savgol_filtd) < mThresh)[0]
            i1 = thCon[-1]
            i2 = i1+1
            if i2 >= len(dkc['Ti']):
                return None
            T_ic = dkc['Ti'][i2] - (dkc['<m>'][i2] - mThresh) * \
                    (dkc['Ti'][i2] - dkc['Ti'][i1]) / \
                    (dkc['<m>'][i2] - dkc['<m>'][i1])
            T_icvar = (dkc['<m>'][i2] - dkc['<m>'][i1])/2
        elif self.thMOD in ['MIDPOINT', 'LINEFIT']:
            thCon = where(array(dkc['<m>']) < mThresh)[0]
            if not thCon.size:
                return None
            i1 = thCon[-1]
            i2 = i1+1
            if i2 >= len(dkc['Ti']):
                return None
            if self.thMOD == 'MIDPOINT':
                T_ic = dkc['Ti'][i2] - (dkc['<m>'][i2] - mThresh) * \
                    (dkc['Ti'][i2] - dkc['Ti'][i1]) / \
                    (dkc['<m>'][i2] - dkc['<m>'][i1])
                T_icvar = (dkc['<m>'][i2] - dkc['<m>'][i1])/2
                # errorT_ic = sqrt((abs(dkc['<m>'][i1]+mSompolinsky-2*dkc['<m>'][i2])/(dkc['Ti'][i2] - dkc['Ti'][i1])*)**2)
            elif self.thMOD == 'LINEFIT':
                m = (dkc['<m>'][i2] - dkc['<m>'][i1]) / \
                    (dkc['Ti'][i2] - dkc['Ti'][i1])
                x0 = dkc['Ti'][i1]
                c = dkc['<m>'][i1]
                #f = lambda x: m * (x-x0) - c
                def fm1(y): return (y-c)/m + x0
                T_ic = fm1(mThresh)
                T_icvar = (dkc['<m>'][i2] - dkc['<m>'][i1])/2
        if T_ic - T_icvar < 0:
            T_icvar = T_icvar - abs((T_ic - T_icvar))
        return T_ic, T_icvar

    def MAKE__T_iavgmperK(self, d, K):
        dkc = self.MAKE__dkc(d['N'], d['Th'], K,
                             self.GET__dXvk(d['TiK'], 'K', K))
        return zip(array(dkc['Ti']), abs(array(dkc['<m>'])))

    def MAKE__KavgmperT_i(self, d, Ti):
        dkc = self.MAKE__dkc_T_i(d['N'], d['Th'], Ti,
                             self.GET__dXvk(d['TiK'], 'Ti', Ti))
        return zip(array(dkc['K']), abs(array(dkc['<m>'])))

    def MAKE__avgmperK(self, d, N, th, K):
        dkc = self.MAKE__dkc(
            N, th, K, self.GET__dXvk(d['Ti/K'], 'K', K))
        return array(dkc['<m>'])

    def MAKE__T_iperK(self, d, N, th, K):
        dkc = self.MAKE__dkc_nom(K, self.GET__dXvk(d['TiK'], 'K', K))
        return array(dkc['Ti'])

    def MAKE__alpha_c(self, Tvsa):
        if self.Hp > 2:
            popt, *_ = curve_fit(self.βcfit, Tvsa[:, 0], 1/Tvsa[:, 1], 
                              sigma=sqrt(Tvsa[:,2]/Tvsa[:,1]))
            return popt[0]
        else:
            return αcStiefvater
