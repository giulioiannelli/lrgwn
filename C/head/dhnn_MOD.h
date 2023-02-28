#define __DHNN_V_ALPH__
/* BOOLEAN OPTIONS ---------------------------------------------------------- */

#define ISING1D 0
#define MARKOVP 1

#define JCOUP2 1
#define JCOUP3 0

#define HOPF2 0
#define HOPF3 0

#define JREAD 0
#define JCALC 1

#define HOPP 2

/* OTHER OPTIONS ------------------------------------------------------------ */
#define rnd_int SFMT_rnd_uint64_t
#define rnd_dbl SFMT_rnd

#define SFFX "_renormH2"

#define JI 1.
#define TH 0
#define NOISET 0
#if NOISET
#define NOISEP 1e-2
#endif
#define PTHRESH 1e-3
#define AVG_M 10

#define HOPS "2"
#define MCTN round(sqrt(N))
#define KNUM ((uint64_t)AVG_M * N)

#if (HOPP == 3)
#undef HOPS
#define HOPS "3"
#undef KNUM
#define KNUM ((uint64_t)AVG_M * N * N)
#endif
