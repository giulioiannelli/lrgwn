/*H*__C program file__**********************************************************
 * FILENAME: dhnn.c                                          DESIGN REF: DHNNC01
 *                                                              VERSION: A.01.00
 *
 * DESCTIPTION:
 *      This is the main program of the project "Dense Associative Memories and
 *      spatial correlations". The body of the program contains all the
 *      functions that are useful to
 *        (1) Generate spatially correlated patterns [c(i, j)] uncorrelated one
 *          with the other with two methods (as patterns coming from a
 *          Metropolis simulation of the Ising linear chain, or as a result of
 *          a stochastic process with the same correlation function as the
 *          one dimensional Ising model).
 *        (2) Generate the coupling matrices for Hopfield Networks or
 *          generalized Hopfield Networks (p>2) when arising from the pattern
 *          at point (1).
 *        (3) Run the Hopfield model with such coupling matrices and measure the
 *          Mattis magnetization at the end of the simulation.
 *
 * PUBLIC FUNCTIONS:
 *      see -> head/dhnnlib.h
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
 * COMPILATION:
 *   gcc -O3 -DSFMT_MEXP=19937 -o exe/a.o C/head/SFMT/SFMT.c C/dhnn.c -Wall -lm
 * EXECUTION:
 *   nohup exe/a.o in/exampleinput.txt >/dev/null 2>&1 &
 *
 * AUTHOR: Giulio Iannelli =========================== START DATE:   01 sep 2022
 *
 * CHANGES:
 *
 * REF NO   VERSION     DATE        WHO     DETAIL
 * C01/01   A.00.00     01sep22     GI      created program
 * C01/02   A.00.01     20sep22     GI      major aesthetic rev.
 * C02/01   A.01.00     04nov22     GI      merging with dual
 *
 *H*/
#include "head/dhnnlib.h"

#define NARG 1 + 1
#define PROGN NJCO2

int main(int argc, char *argv[])
{
    /* make logfile - check tree directory - compile exec. - check argc */
    check_argc(argc, NARG);
    make_dir(DIRLOGF);
    sprintf(buf, DIRLOGF NDHNN _H "[%s]" EXTLOG, argv[1] + strlen(DIRINPT));
    make_log(argc, buf, NDHNN, argv);
    make_log_dhnn();
    check_dirtree();
    gcc_compall();
    /* open input file for reading [skip header] - loop over input variables */
    FILE *f_in, *f_r;
    finputopen(&f_in, argv[1]);
#ifdef __DHNN_V_TEMP__
    while (fscanf(f_in, "%" SCNu32 " %lf %lf", &N, &Ti, &Th) != EOF)
    {
        /* strings for execv arguments */
        printf("debug\n");
        strlen_u16 = snprintf(NULL, 0, STR_FMT_T, Ti) + 1;
        strTi = malloc(strlen_u16);
        snprintf(strTi, strlen_u16, STR_FMT_T, Ti);
#else
    FILE *f_inb;
    while (fscanf(f_in, "%" SCNu32 " %lf %lf", &N, &Al, &Th) != EOF)
    {
        strlen_u16 = snprintf(NULL, 0, STR_FMT_A, Al) + 1;
        strAl = malloc(strlen_u16);
        snprintf(strAl, strlen_u16, STR_FMT_A, Al);
#endif
        strlen_u16 = snprintf(NULL, 0, STR_FMT_N, N) + 1;
        strN = malloc(strlen_u16);
        snprintf(strN, strlen_u16, STR_FMT_N, N);
        strlen_u16 = snprintf(NULL, 0, STR_FMT_TH, Th) + 1;
        strTh = malloc(strlen_u16);
        snprintf(strTh, strlen_u16, STR_FMT_TH, Th);
        printf("debug\n");
        /* pattern generation -- MODES: >ISING1D< or >MARKOVP< */
#ifdef __DHNN_V_ALPH__
        sprintf(buf, DIRTALP __AIS__ _U __NIS__ EXTTXT, Al, N);
        // sprintf(buf, DIRINPT TPFXH HOPS _U STR_FMT_A EXTTXT, Al);
        if ((f_inb = fopen(buf, "r+")) == NULL)
        {
            fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
            exit(EXIT_FAILURE);
        }
        while (fscanf(f_inb, "%lf", &Ti) != EOF)
        {
            strlen_u16 = snprintf(NULL, 0, STR_FMT_T, Ti) + 1;
            strTi = malloc(strlen_u16);
            snprintf(strTi, strlen_u16, STR_FMT_T, Ti);
#endif
            /* pattern generation -- MODES: >ISING1D< or >MARKOVP< */
            sprintf(buf, DIRCFGS P_TYPE _UNISUTIS_ _U __KNUMIS__ EXTBIN,
                    N, Ti, KNUM);
            /* check KNUM configuration exists, otherwise generate */
            printf("debug\n");
            if (F_NEXIST(buf))
            {
                fprintf(f_log, MSGINFO PIFNXST "%s", buf);
#if ISING1D
                char *args1DK[] = {(char *)NI1DK, strN, strTi, NULL};
                run_sub(3, args1DK);
#elif MARKOVP
                char *argsMarkov[] = {(char *)NMC1D, strN, strTi, NULL};
                run_sub(3, argsMarkov);
#endif
            }
            /* reading check/check */
            sprintf(buf, DIRCHCK P_TYPES _UNISUTIS_ EXTTXT, N, Ti);
            if ((f_r = fopen(buf, "r")) == NULL)
            {
                fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
                exit(EXIT_FAILURE);
            }
            else
            {
                sysret = fscanf(f_r, "%lf", &check);
                fclose(f_r);
            }
            /* skip current input param. if THCOND is false */
            if THCOND
            {
                FREE_VALSET_STR2();
                fprintf(f_log, MSGWARN PWEXDTH PISKPCV __NIS__ _C __TIS__ _C __THIS__, N, Ti, Th);
                continue;
            }
#ifdef __DHNN_V_ALPH__
            free(strTi);
        }
        fclose(f_inb);
#endif
#if JCOUP2
        /* create coupling matrices */
        fprintf(f_log, MSGINFO PIFNXST "%s", buf);
        char *argsJ2[] = {(char *)NJCO2, strN, STRTiAl, NULL};
        run_sub(3, argsJ2);
#endif
#if JCOUP3
        /* create coupling matrices */
        fprintf(f_log, MSGINFO PIFNXST "%s", buf);
        char *argsJ3[] = {(char *)NJCO3, strN, STRTiAl, NULL};
        run_sub(3, argsJ3);
#endif
#if HOPF2
        char *argsH2[] = {(char *)NHOP2, strN, STRTiAl, strTh, NULL};
        run_sub(4, argsH2);
#endif
#if HOPF3
        char *argsH3[] = {(char *)NHOP3, strN, STRTiAl, strTh, NULL};
        run_sub(4, argsH3);
#endif
        FREE_VALSET_STR();
    }
    fclose(f_log);
    return 0;
}