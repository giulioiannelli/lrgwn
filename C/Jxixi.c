/*H*__C program file__**********************************************************
 * FILENAME: Jxixi.c                                         DESIGN REF: JXIXI00
 *                                                              VERSION: A.00.00
 *
 * DESCTIPTION:
 *      This program generates matrices obtained by outer product of K Ising
 *      linear chain configurations at a temperature T. The result are then
 *      stored in a binary file as the lower triangular part of the matrix,
 *      since it is symmetric.
 *      INPUT: N    K   T
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
 *   gcc -O3 -DSFMT_MEXP=19937 -o exe/Jxixi.o C/head/SFMT/SFMT.c C/Jxixi.c
 *          -Wall -lm
 *
 * AUTHOR: Giulio Iannelli =========================== START DATE:   01 sep 2022
 *
 * CHANGES:
 *
 * REF NO   VERSION     DATE        WHO     DETAIL
 * C00/00   A.00.00     02dic22     GI      created program
 *
 *H*/
#include "head/dhnnlib.h"

#define NARG 1 + 3
#define PROGN NJXIX

#define LSEED1 0xABACAB
#define LSEED2 0xBAFF00

int main(int argc, char *argv[])
{
    /* check argc - make logfile - initialize the sfmt rng */
    check_argc(argc, NARG);
    sprintf(buf, DIRLOGF PROGN _U __NIS "%s" _U __TIS "%s" _U __KIS "%s" EXTLOG,
            argv[1], argv[2], argv[3]);
    make_log(argc, buf, PROGN, argv);
    check_dirtree();
    srand(time(0) ^ getpid());
    seed_rand[2] = (LSEED1*rand()) % UINT32_MAX;
    seed_rand[3] = (LSEED2*rand()) % UINT32_MAX;
    sfmt_init_by_array(&sfmt, seed_rand, LENSRND);
    /* variable declaration - acquisition from command line */
    FILE *f_out;
    /* get variable from command line */
    N = strtoul_chck(argv[1], 10);
    T = strtod_chck(argv[2]);
    K = strtoul_chck(argv[3], 10);
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
        fclose(f_out);
        fclose(f_log);
        exit(EXIT_SUCCESS);
    }
    fclose(f_out);
    /* open the file */
    /* generate missingK configuration */
    s = calloc(N, sizeof(*s));
    challoc(s);
    xi = malloc(sizeof(*xi) * K);
    challoc(xi);
    for (uint32_t mu = 0; mu < K; mu++)
    {
        xi[mu] = malloc(sizeof(**xi) * N);
        challoc(xi[mu]);
    }
    for (uint32_t mu = 0; mu < K; mu++)
    {
        s[0] = SGN01(rnd_dbl() - .5);
        for (uint32_t i = 1; i < N; i++)
        {
            if (rnd_dbl() < PFLIP)
                s[i] = s[i - 1];
            else
                s[i] = -s[i - 1];
        }
        memcpy(xi[mu], s, sizeof(*s) * N);
    }
    sprintf(buf, DIRJMA2 __NIS__ _U __TIS__ _U __KIS__ EXTBIN, N, T, K);
    if ((f_out = fopen(buf, "wb")) == NULL)
    {
        fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < N; i++)
    {
        for (uint32_t j = 0; j < i; j++)
        {
            tmp_i32 = 0;
            for (uint32_t mu = 0; mu < K; mu++)
                tmp_i32 += xi[mu][i] * xi[mu][j];
            fwrite(&tmp_i32, sizeof(tmp_i32), 1, f_out);
        }
    }
    tmp_u32 = K;
    while (tmp_u32)
        free(xi[--tmp_u32]);
    free(xi);
    free(s);
    fclose(f_log);
    fclose(f_out);
}
 