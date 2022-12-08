/*H*__C header file__***********************************************************
 * FILENAME: dhnnlib.h                                       DESIGN REF: DHNNH00
 *                                                              VERSION: A.00.02
 *
 * DESCTIPTION:
 *     ... TO DO ...
 *
 * INCLUDED LIBRARIES:
 *      ... TO DO ...
 *
 * PUBLIC FUNCTIONS:
 *      ... TO DO ...
 *
 * NOTES:
 *      ...
 *
 *      SYSTEM SETTINGS ------------------------------------|
 *        |  os: Ubuntu 22.04.1 LTS x86_64                  |
 *        |  kernel: Linux 5.15.0-47-generic                |
 *        |  gcc version 11.2.0 (Ubuntu 11.2.0-19ubuntu1)   |
 *        --------------------------------------------------|
 *
 *      Copyleft G. Iannelli 2022.          No rights reserved.
 *          email: giulioiannelli.w@gmail.com
 *
 *
 * AUTHOR: Giulio Iannelli =========================== START DATE:   01 sep 2022
 *
 * CHANGES:
 *
 * REF NO   VERSION     DATE        WHO     DETAIL
 * C01/00   A.00.00     01sep22     GI      created program
 * C01/01   A.00.01     20sep22     GI      major aesthetic rev.
 * C01/02   A.00.02     23sep22     GI      edit on fixbranch
 *
 *H*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "SFMT/SFMT.h"
#include "dhnn_MOD.h"
#include "dhnn_UTILS.h"

#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wformat"

#define MAXSTRLEN 512
#define LENSRND 4
#define SEED (uint32_t)0xBE11AC1A0
#define SIID (uint32_t)0xC1A0BE11A

#define NSQ (N * N)
#define ALPHA_c 0.144

#define DUMP(x) (#x)
#define SGN01(x) (x < 0 ? -1 : 1)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

FILE *f_log;
signed char *s, **xi;;
char *strN, *strT, *strAl, *strTh, *endptr;
int sysret;
uint16_t strlen_u16;
uint32_t N, K, tmp_u32;
uint32_t seed_rand[] = {SEED, SIID, 0, 0};
int32_t tmp_i32;
TMPPTR_T *tmpntr;
double check;
double T, Th, Al;
sfmt_t sfmt;

char bufF[MAXSTRLEN], buf[MAXSTRLEN], buf2[MAXSTRLEN], buf3[MAXSTRLEN];

/* Compile a program with the inclusion of SFMT Mersenne Twister RNG.
 * The ouput program has the same name of the input with suffix spcified by
 * preprocessor directive SFFX.
 *
 * Options can be specified by means of argument (char *) opt. GCCWALLMO3 is
 * equivalent to "-Wall -lm -O3".
 */
void gcc_comp(char *cname, char *opt)
{
    sprintf(bufF, GCCDSFMTEXE "%1$s" SFFX GCCEXESFMTDIRC "%1$s" EXTCPG " %2$s",
            cname, opt);
    if (!(system(bufF) + 1))
    {
        fprintf(f_log, MSGFAIL PFGCCMP "%s" EXTCPG MSGEXIT, cname);
        exit(EXIT_FAILURE);
    }
}
/* Compile all C programs in folder DIRCPRO with options GCCWALLMO3.
 */
void gcc_compall(void)
{
#if ISING1D
    gcc_comp(NI1DF, GCCWALLMO3);
    gcc_comp(NI1DT, GCCWALLMO3);
    gcc_comp(NI1DK, GCCWALLMO3);
#endif
#if MARKOVP
    gcc_comp(NMC1D, GCCWALLMO3);
#endif
#if JCOUP2
    gcc_comp(NJCO2, GCCWALLMO3);
#endif
#if JCOUP3
    gcc_comp(NJCO3, GCCWALLMO3);
#endif
#if HOPF2
    gcc_comp(NHOP2, GCCWALLMO3);
#endif
#if HOPF3
    gcc_comp(NHOP3, GCCWALLMO3);
#endif
}
/* Run executable file with arguments from command line specified by
 * (char **) args.
 *
 * Then print on file f_log whether the program did succesfully or not, along
 * with input arguments.
 */
void run_sub(int larg, char **args)
{
    sprintf(bufF, DIREXEC "%s" SFFX EXTEXE, args[0]);
    pid_t pid = fork();
    if (pid == 0)
    {
        execv(bufF, args);
        fprintf(f_log, MSGFAIL PFEXECV "%s" MSGEXIT, bufF);
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        fprintf(f_log, MSGFAIL PFEXECV MSGEXIT);
        exit(EXIT_FAILURE);
    }
    else
        waitpid(pid, 0, 0);
    fprintf(f_log, MSGEXEC "%s", args[0]);
    for (int i = 1; i < larg; i++)
        fprintf(f_log, " %s", args[i]);
}
/* Open LOG file and initialize it with program name and argument from CLI.
 */
void make_log(int argc, char *buf, const char *progn, char *argv[])
{
    if ((f_log = fopen(buf, "w+")) == NULL)
    {
        printf(MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    }
    fprintf(f_log, LOGHEAD "%s" EXTCPG MSGINFO, progn);
    for (int i = 0; i < argc; i++)
        fprintf(f_log, (i < argc - 1 ? "%s " : "%s"), argv[i]);
}
/* Print on the LOG file all the options defined in dhnn_MOD.h
 */
void make_log_dhnn(void)
{
    fprintf(f_log, LOGHEA2);
#ifdef __DHNN_V_ALPH__
    fprintf(f_log, LOGHEA3 "%s", DUMP(__DHNN_V_ALPH__));
#elif defined(__DHNN_V_TEMP__)
    fprintf(f_log, LOGHEA3 "%s", DUMP(__DHNN_V_TEMP__));
#endif
    fprintf(f_log, LOGPPDO "%s" _TTSARW SFFX, DUMP(SFFX));
    fprintf(f_log, LOGPPDO "%s" _TTSARW P_TYPE, DUMP(P_TYPE));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%g", DUMP(JI), JI);
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%g", DUMP(TH), (double)TH);
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%d", DUMP(AVG_M), AVG_M);
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%g", DUMP(MCTN), MCTN);
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(NOISET), LOGONOFF(NOISET));
#if NOISET
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%g", DUMP(NOISET), LOGONOFF(NOISET));
#endif
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(ISING1D), LOGONOFF(ISING1D));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(MARKOVP), LOGONOFF(MARKOVP));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(JCOUP2), LOGONOFF(JCOUP2));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(JCOUP3), LOGONOFF(JCOUP3));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(HOPF2), LOGONOFF(HOPF2));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(HOPF3), LOGONOFF(HOPF3));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(JREAD), LOGONOFF(JREAD));
    fprintf(f_log, LOGPPDO "%s" _TTSARW "%s", DUMP(JREAD), LOGONOFF(JCALC));
}
/*
 */
uint32_t strtoul_chck(char *inpt, int base)
{
    uint32_t tmp;
    tmp = strtoul(inpt, &endptr, base);
    if (*endptr != '\0' || errno != 0)
    {
        fprintf(f_log, MSGFAIL PFCLU32 PIGOTIN "%s" MSGEXIT, endptr);
        exit(EXIT_FAILURE);
    }
    return tmp;
}
uint32_t strtoull_chck(char *inpt, int base)
{
    uint32_t tmp;
    tmp = strtoull(inpt, &endptr, base);
    if (*endptr != '\0' || errno != 0)
    {
        fprintf(f_log, MSGFAIL PFCLU64 PIGOTIN "%s" MSGEXIT, endptr);
        exit(EXIT_FAILURE);
    }
    return tmp;
}
/*
 */
double strtod_chck(char *inpt)
{
    double tmp;
    tmp = strtod(inpt, &endptr);
    if (*endptr != '\0' || errno != 0)
    {
        fprintf(f_log, MSGFAIL PFCLDBL PIGOTIN "%s" MSGEXIT, endptr);
        exit(EXIT_FAILURE);
    }
    return tmp;
}
/*
 */
void challoc(void *__ptr)
{
    if (__ptr == NULL)
    {
        fprintf(f_log, MSGFAIL PFMEMAL "%s" MSGEXIT, DUMP(__ptr));
        exit(EXIT_FAILURE);
    }
}
/* Check that the number of arguments passed from CLI corresponds to narg.
 */
void check_argc(int argc, int narg)
{
    if (argc != narg)
    {
        fprintf(f_log, MSGFAIL PFNARGC MSGEXIT);
        exit(EXIT_FAILURE);
    }
}
/* Create with mkdir bash command a new folder with permissions set by
 * ACCESSPERMS (0777, i.e. rwxrwxrwx). Then reset errno to 0.
 */
void make_dir(char *dname)
{
    mkdir(dname, ACCESSPERMS);
    errno = 0;
}
/* Same as make_dir but more checking and printing on log file potential errors.
 */
void make_dir_flog(char *dname)
{
    mkdir(dname, ACCESSPERMS);
    fprintf(f_log, PIMKDIR "%s\t(%s)" PIRESER, dname, strerror(errno));
    errno = 0;
}
/* Make directories for hopfield subcases, that is for each set of N, T and Th.
 * The function sprintf on the global buf so that the variable remains sprinted
 * until further sprintf.
 */
void make_dirhop(const char *hoppb)
{
    sprintf(buf, "%s" P_TYPE "_N=%" PRIu32 "_avg=%" PRIu32 "/", hoppb, N, AVG_M);
    make_dir_flog(buf);
    sprintf(buf + strlen(buf), "T_h=%g/", Th);
    make_dir_flog(buf);
}
/* Check the whole directory tree and report
 */
void check_dirtree(void)
{
    for (int i = 0; i < ARRAY_SIZE(dirarr); i++)
        make_dir_flog(dirarr[i]);
}
void finputopen(FILE **fin, char *fname)
{
    if ((*fin = fopen(fname, "r")) == NULL)
    {
        fprintf(*fin, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    }
    else
        sysret = fscanf(*fin, "%*[^\n]");
}

/* ################ function: void FREE_VALSET_STR() ---------------------------
 * <<< descr. >>> free all strings for parsing arguments to child processes.
 * ++ return +++ void
 */
void FREE_VALSET_STR()
{
    free(strN);
    free(strTh);
#ifdef __DHNN_V_TEMP__
    free(strT);
#elif defined(__DHNN_V_ALPH__)
    free(strAl);
#endif
}
void FREE_VALSET_STR2()
{
    free(strN);
    free(strTh);
#ifdef __DHNN_V_TEMP__
    free(strT);
#elif defined(__DHNN_V_ALPH__)
    free(strAl);
    free(strT);
#endif
}
/* ################ function: bool F_EEXIST(const char *) ----------------------
 * <<< descr. >>> check if a FILE, whose PATH is given by string "n", exists.
 * ++ return +++ boolean variable depending on the existence of FILE.
 */
bool F_EEXIST(const char *n)
{
    FILE *f;
    if ((f = fopen(n, "r")))
        fclose(f);
    else
        return false;
    return true;
}
/* ################ function: bool F_NEXIST(const char *) ----------------------
 * <<< descr. >>> check if a FILE, whose PATH is given by string "n", nexists.
 * ++ return +++ boolean variable depending on the non existence of FILE.
 */
bool F_NEXIST(const char *name)
{
    return (!(F_EEXIST(name)));
}
/* ################ function: double rnd(void) ---------------------------------
 * <<< descr. >>> generate a random number with rand() 32-bit function from
 * <stdlib.h>. The generated number is in [0, 1]
 * ++ return +++ the double random number generated.
 */
double rnd()
{
    return (double)rand() / RAND_MAX;
}
/* ################ function: double SFMT_rnd(void) ----------------------------
 * <<< descr. >>> generate a random number with the fast Mersenne Twister
 * algorithm. The generated random number on [0,1) has a 53-bit resolution
 * using sfmt_genrand_uint64 function.
 * ++ return +++ the double random number generated.
 */
double SFMT_rnd()
{
    return sfmt_genrand_res53(&sfmt);
}
/* ################ function: uint64_t SFMT_rnd_uint64_t() ---------------------
 * <<< descr. >>> 64-bit pseudorandom number using SMFT.h function
 * sfmt_genrand_uint64. init_gen_rand or init_by_array must be called before
 * this function. The function gen_rand32 should not be called after
 * gen_rand32, unless an initialization is again executed.
 * ++ return +++ uint64_t random number generated.
 */
uint64_t SFMT_rnd_uint64_t()
{
    return sfmt_genrand_uint64(&sfmt);
}
/* ################ function: bool frozen(uint32_t, signed char *) -------------
 * <<< descr. >>>
 * ++ return +++
 */
bool s_frozen(uint32_t N, signed char *s)
{
    int32_t sum = 0;
    for (uint32_t i = 0; i < N; i++)
        sum += s[i];
    if (labs(sum) == N)
        return true;
    else
        return false;
}

void s_init(uint32_t N, signed char *s)
{
    for (uint32_t i = 0; i < N; i++)
        s[i] = 2 * (rnd_int() % 2) - 1;
}
void s_upd(uint32_t N, signed char *s)
{
    int32_t rnds, ln, rn;
    double w, r;

    for (uint32_t i = 0; i < N; i++)
    {
        r = rnd_dbl();
        rnds = rnd_int() % N;
        ln = rnds - 1;
        rn = rnds + 1;
        s[rnds] = -s[rnds];
        if (ln == -1)
            ln += N;
        else if (rnds == N - 1)
            rn -= N;
        w = exp(2 * 1 / T * JI * s[rnds] * (s[ln] + s[rn]));
        if (r > w)
            s[rnds] = -s[rnds];
    }
}
double s_magn(uint32_t LEN, signed char *s)
{
    double sum = 0.;
    for (uint32_t i = 0; i < LEN; i++)
        sum += s[i];
    return sum / LEN;
}
double m_avg_t(uint32_t LEN, double *arr)
{
    double sum = 0.;
    for (uint32_t i = 0; i < LEN; i++)
        sum += arr[i];
    return sum / LEN;
}
double m_avg2_t(uint32_t LEN, double *arr)
{
    double sum = 0.;
    for (uint32_t i = 0; i < LEN; i++)
    {
        sum += arr[i] * arr[i];
    }
    return sum / LEN;
}
double m_corr_t(uint32_t LEN, uint32_t l, double *arr)
{
    double sum = 0.;
    for (uint32_t t = 0; t < LEN - l; t++)
        sum += arr[t] * arr[t + l];
    return sum / (LEN - l);
}
double upd_prob(double T, double h)
{
    return .5 * (1. + tanh(h / T));
}
signed char upd_s(double h_i, signed char supd)
{
#if TH
    h_i /= .5 * N;
    if (rnd_dbl() < upd_prob(Th, h_i))
        return = +1;
    else
        return = -1;
#else
    if (h_i * supd < 0)
        supd = -supd;
    return supd;
#endif
}

double beta_cH2(double a)
{
    return atanh(sqrt(2 * (ALPHA_c - a) / (a * a - 2 * a * ALPHA_c + 2 * ALPHA_c + a * sqrt(a * a + 4 * (1 - ALPHA_c)))));
}
void find_CFGS()
{
    FILE *pipe;
    sprintf(buf, "find " DIRCFGS " -maxdepth 1 -name " P_TYPE _U __NIS__ _U __TIS__ _U "*",
        N, T);
    if ((pipe = popen(buf, "r")) == NULL)
    {
        perror("PIPING ERROR");
        exit(EXIT_FAILURE);
    }
    fgets(buf, sizeof(buf), pipe);
    buf[strlen(buf) - 1] = '\0';
    pclose(pipe);
}