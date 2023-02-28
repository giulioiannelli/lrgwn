/*H*__C program file__**********************************************************
 * FILENAME: i1Dmkvp.c                                       DESIGN REF: I1DMP01
 *                                                              VERSION: A.01.00
 *
 * DESCTIPTION:
 *      ...
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
 *   gcc -O3 -DSFMT_MEXP=19937 -o exe/a.o C/head/SFMT/SFMT.c C/i1Dmkvp.c
 *          -Wall -lm
 * EXECUTION:
 *   nohup exe/a.o in/exampleinput.txt >/dev/null 2>&1 &
 *
 * AUTHOR: Giulio Iannelli =========================== START DATE:   01 sep 2022
 *
 * CHANGES:
 *
 * REF NO   VERSION     DATE        WHO     DETAIL
 * C01/01   A.01.00     04nov22     GI      append to existing cfgs files
 *
 *H*/
#include "head/dhnnlib.h"

#define NARG 1 + 2
#define PROGN NMC1D

#define LSEED1 0xBBBBBB
#define LSEED2 0x1BACF5

int main(int argc, char *argv[])
{
    /* check argc - make logfile - initialize the sfmt rng */
    check_argc(argc, NARG);
    sprintf(buf, DIRLOGF PROGN _U __NIS "%s" _U __TIS "%s" EXTLOG,
            argv[1], argv[2]);
    make_log(argc, buf, PROGN, argv);
    seed_rand[2] = LSEED1;
    seed_rand[3] = LSEED2;
    sfmt_init_by_array(&sfmt, seed_rand, LENSRND);
    /* variable declaration - acquisition from command line */
    FILE *f_out;
    uint64_t effKNUM, missingK;
    /* get variable from command line */
    N = strtoul_chck(argv[1], 10);
    T = strtod_chck(argv[2]);
    /* check pflip */
    sprintf(buf, DIRCHCK P_TYPES _U __NIS__ _U __TIS__ EXTTXT, N, T);
    if ((f_out = fopen(buf, "w+")) == NULL)
    {
        fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    }
    fprintf(f_out, "%lf", PFL1P);
    if (PFL1P < PTHRESH)
    {
        fprintf(f_log, MSGWARN PWFLPTH "%g" MSGEXIT, PTHRESH);
        exit(EXIT_SUCCESS);
        fclose(f_out);
        fclose(f_log);
    }
    fclose(f_out);
    /* find the file, if exists, */
    find_CFGS();
    printf("%s\n", buf);
    if (!(strncmp(buf, STR_FIND, strlen(STR_FIND))))
    {
        missingK = KNUM;
        effKNUM = KNUM;
    }
    else if (!(strncmp(buf, DIRCFGS, strlen(DIRCFGS))))
    {
        sprintf(buf2, DIRCFGS P_TYPE _U __NIS__ _U __TIS__ _U "[", N, T);
        strncpy(buf3, buf + strlen(buf2), strlen(buf) - strlen(buf2) - 5);
        effKNUM = strtoull_chck(buf3, 10);
        if (effKNUM >= KNUM)
        {
            return 0;
        }
        missingK = KNUM - effKNUM;
    }
    else
    {
        exit(EXIT_FAILURE);
    }
    /* open the file */
    sprintf(buf, DIRCFGS P_TYPE _U __NIS__ _U __TIS__ _U __KNUMIS__ EXTBIN,
            N, T, effKNUM);
    if ((f_out = fopen(buf, "a+b")) == NULL)
    {
        printf(MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    }
    fseek(f_out, 0, SEEK_END);
    /* generate missingK configuration */
    for (uint32_t k = 0; k < missingK; k++)
    {
        s = calloc(N, sizeof(*s));
        s[0] = SGN01(rnd_dbl() - .5);
        for (uint32_t i = 1; i < N; i++)
        {
            if (rnd_dbl() < PFLIP)
                s[i] = s[i - 1];
            else
                s[i] = -s[i - 1];
        }
        fwrite(s, sizeof(*s), N, f_out);
        free(s);
    }
    if (effKNUM < KNUM)
    {
        sprintf(buf2, DIRCFGS P_TYPE _U __NIS__ _U __TIS__ _U __KNUMIS__ EXTBIN,
                N, T, KNUM);
        if ((rename(buf, buf2)) == 0)
        {
            fprintf(f_log, PIFRSUC);
        }
        else
        {
            fprintf(f_log, MSGFAIL PFUNREN);
        }
    }
    fclose(f_log);
    fclose(f_out);
}
 