#define _S "/"
#define _U "_"
#define _H "-"
#define _C ","
#define _NN "\n\n"
#define _NT "\n\t"
#define _NTSV "\n\t |"
#define _TT "\t\t"
#define _NTSVT "\n\t |\t"
#define _NSWTVT "\n !\t |\t"
#define _SARW "-> "
#define _LARW "--> "
#define _TTSARW _TT _SARW
#define _NVNVNVNVA _NTSVT _NTSVT _NTSVT _NTSV _LARW

#define _HLIFI "{+}"
#define _LLPFI "[?]"
#define _LLPWI "[!]"
#define _LLPII "[+]"
#define _LLPEI "[*]"
#define _LLPPI "[•]"

#define LOGHEAD _HLIFI " LOG FILE OF ----------------------------|"
#define LOGFOOT _HLIFI " END OF LOG FILE ------------------------|" 
#define LOGPPDO _NT _LLPPI " "
#define LOGHEA2 LOGPPDO "PREPROCESSOR DIRECTIVES: "
#define LOGHEA3 LOGPPDO "WORKING IN SETTING: "
#define LOGONN "|ON |"
#define LOGOFF "|OFF|"
#define LOGONOFF(x) (x ? LOGONN : LOGOFF)

#define MSGFAIL _NN _LLPFI " PROGRAM FAILURE: "
#define MSGWARN _NN _LLPWI " WARNING: "
#define MSGINFO _NT _LLPII " INFO: "
#define MSGEXEC _NT _LLPEI " EXECUTED: "
#define MSGEXIT _NVNVNVNVA "EXITING THE PROGRAM" _NN

#define PFCLU64 _NTSVT "expected uint64 type as command line input."
#define PFCLU32 _NTSVT "expected uint32 type as command line input."
#define PFCLDBL _NTSVT "expected double type as command line input."
#define PFEXECV _NTSVT "error executing (execv) " _SARW
#define PFFOPEN _NTSVT "error opening file " _SARW
#define PFGCCMP _NTSVT "error gcc-compiling " _SARW
#define PFMEMAL _NTSVT "error allocating memory " _SARW
#define PFNARGC _NTSVT "wrong number of arguments (argc) passed."
#define PFNKNUM _NTSVT "not enough KNUM patterns for coupling matrices."
#define PFUNREN _NTSVT "unable to rename the file."

#define PWFRZNS _NSWTVT "configurations reached frozen state."
#define PWFLPTH _NSWTVT "flip probability less than flip threshold."
#define PWTAUTH _NSWTVT "tauint is less than 1 or couldnt be established."
#if ISING1D
#define PWEXDTH PWTAUTH
#elif MARKOVP
#define PWEXDTH PWFLPTH
#endif

#define PIFEXST _NTSVT "file already exists " _SARW
#define PIFNXST _NTSVT "file did not exist " _SARW
#define PIFRSUC _NTSVT "file renamed successfully."
#define PIGOTIN _NTSVT "got instead: "
#define PIMKDIR _NTSVT "created directory (w ERRNO) " _SARW
#define PIREADF _NTSVT "reading from file " _SARW
#define PIRESER _NTSVT "resetting errno to 0."
#define PISKPCV _NTSVT "skipping current set of values."
#define PICJMOK _NTSVT "coupling matrix done for values" _SARW
#define PICJMNK _NTSVT "coupling matrix exists or 1/b < 1/b_c (SKIP)" _SARW

#define EXTBIN ".bin"
#define EXTLOG ".log"
#define EXTTXT ".txt"
#define EXTDAT ".dat"
#define EXTEXE ".o"
#define EXTCPG ".c"

#ifdef __DHNN_V_TEMP__
#define V_ALPH_SFFX ""
#define STRTiAl strTi
#elif defined(__DHNN_V_ALPH__)
#define V_ALPH_SFFX "_d"
#define STRTiAl strAl
#endif

#define TMPPTR_T double
#define STR_FIND "find"
#define STR_FMT_N "%" PRIu32
#define STR_FMT_K "%" PRIu32
#define STR_FMT_A "%g"
#define STR_FMT_T "%.3g"
#define STR_FMT_TH "%g"
#define STR_FMT_KN "%" PRIu64
#define __AIS "alpha="
#define __KIS "K="
#define __NIS "N="
#define __TIS "T="
#define __THIS "Th="
#define __AIS__ __AIS STR_FMT_A 
#define __KIS__ __KIS STR_FMT_K
#define __NIS__ __NIS STR_FMT_N
#define __TIS__ __TIS STR_FMT_T
#define __THIS__ __THIS STR_FMT_TH
#define __KNUMIS__ "[" STR_FMT_KN "]"
#define _UNISUTIS_ _U __NIS__ _U __TIS__

#define NDHNN "dhnn"
#define NJCO2 "jmat2" V_ALPH_SFFX
#define NJCO3 "jmat3" V_ALPH_SFFX
#define NHOP2 "hop2" V_ALPH_SFFX
#define NHOP3 "hop3" V_ALPH_SFFX
#define NSNAE "sna_est"
#define NI1DK "i1DKcfg"
#define NI1DF "i1Dfcfg"
#define NI1DT "i1Dtaui"
#define NMC1D "i1Dmkvp"
#define NJXIX "Jxixi"

#define KPFX "alpha" HOPS "-T"
#define TPFX "T-alpha" HOPS

#define DIRMAIN "./"
#define DIRCPRO "C/"
#define DIREXEC "exe/"
#define DIRINPT "in/"
#define DIRPYPR "py/"
#define DIRRESF "res/"
#define DIRLOGF "log/"
#define DIRTMPF DIRRESF "tmp/"
#define DIRHEAD DIRCPRO "head/"
#define DIRSFMT DIRHEAD "SFMT/"
#define DIRDATA DIRRESF "data/"
#define DIRPLOT DIRRESF "plot/"
#define DIRFCFG DIRDATA "fcfg/"
#define DIRCHCK DIRDATA "chck/"
#define DIRCFGS DIRDATA "cfgs/"
#define DIRHOP2 DIRDATA "hop2/"
#define DIRHOP3 DIRDATA "hop3/"
#define DIRJMA2 DIRDATA "jmat2/"
#define DIRJMA3 DIRDATA "jmat3/"

#ifdef __DHNN_V_TEMP__
#define DIRTALP DIRINPT KPFX _S
#elif defined(__DHNN_V_ALPH__)
#define DIRTALP DIRINPT TPFX _S
#endif

#define P_TYPE "ERROR"
#if ISING1D
#undef P_TYPE
#define P_TYPE "ISING"
#define THCVAR check
#define THCOND (check < 1 || isnan(check))
#elif MARKOVP
#undef P_TYPE
#define P_TYPE "MARKOV"
#define THCOND (check < PTHRESH)

#define PFLIP (.5 * (tanh(JI / T) + 1))
#define PFL1P (1. - PFLIP)

#endif
#define P_TYPES P_TYPE "CHECK"

#define GCCDSFMT "gcc -DSFMT_MEXP=19937 -o "
#define GCCDSFMTEXE GCCDSFMT DIREXEC
#define GCCWALLMO3 " -Wall -lm -O3"
#define GCCSFMT " " DIRSFMT "SFMT" EXTCPG " "
#define GCCEXESFMTDIRC EXTEXE GCCSFMT DIRCPRO

char *dirarr[] = {DIRCPRO, DIREXEC, DIRINPT, DIRRESF, DIRHEAD, DIRSFMT, DIRDATA,
                  DIRFCFG, DIRCHCK, DIRCFGS, DIRHOP2, DIRHOP3, DIRJMA2,
                  DIRJMA3};