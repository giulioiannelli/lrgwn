#include "head/dhnnlib.h"

#define NARG 1 + 2
#define PROGN NJCO2

#define LSEED1 0xFAFAFFE
#define LSEED2 0x187D33D


int main(int argc, char *argv[])
{
    /* make logfile - check tree directory - compile exec. - check argc */
    sprintf(buf, DIRLOGF PROGN _U __NIS "%s" _U __AIS "%s" EXTLOG, argv[1], argv[2]);
    make_log(argc, buf, PROGN, argv);
    check_argc(argc, NARG);
    /* initialize the sfmt random number generator */
    seed_rand[2] = LSEED1;
    seed_rand[3] = LSEED2;
    sfmt_init_by_array(&sfmt, seed_rand, LENSRND);
    /* get input from command line */
    FILE *f_in, *f_out, *f_r;
    int32_t tmp_i32;
    uint32_t tmp, K;
    double *Ti_vals;
    uint32_t Tilen = 0;
    signed char **xi;
    N = strtoul_chck(argv[1], 10);
    Al = strtod_chck(argv[2]);
    K = (uint32_t)(Al * N);
    /* initialize Tilist */
    sprintf(buf, DIRTALP __AIS__ _U __NIS__ EXTTXT, Al, N);
    if ((f_in = fopen(buf, "r+")) == NULL)
    {
        fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    } /* qui si potrebbe pensare a fare un piccolo listato che se non trova il file lo crea... infondo basta eseguire il programma py*/
    Ti_vals = malloc(sizeof(*Ti_vals));
    challoc(Ti_vals);
    while (fscanf(f_in, "%lf", &T) != EOF)
    {
        Ti_vals[Tilen] = T;
        Tilen++;
        tmpntr = realloc(Ti_vals, sizeof(*Ti_vals) * (Tilen + 1));
        challoc(tmpntr);
        Ti_vals = tmpntr;
    }
    fclose(f_in);
    /* generate coupling matrix */
    for (uint32_t kTi = 0; kTi < Tilen; kTi++)
    {
        sprintf(buf, DIRCHCK P_TYPES _U __NIS__ _U __TIS__ EXTTXT, N, Ti_vals[kTi]);
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
        if THCOND
        {
            fprintf(f_log, MSGWARN PWEXDTH PISKPCV __NIS__ ", " __TIS__ ", " __THIS__,
                    N, T, Th);
            continue;
        }
        for (uint32_t m = 0; m < AVG_M; m++)
        {
            /* initialize patterns */
            sprintf(buf, DIRCFGS P_TYPE _U __NIS__ _U __TIS__ _U __KNUMIS__ EXTBIN,
                    N, Ti_vals[kTi], KNUM);
            if ((f_in = fopen(buf, "rb")) == NULL)
            {
                fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
                exit(EXIT_FAILURE);
            }
            xi = malloc(sizeof(*xi) * KNUM);
            challoc(xi);
            for (uint32_t mu = 0; mu < KNUM; mu++)
            {
                xi[mu] = malloc(sizeof(**xi) * N);
                challoc(xi[mu]);
                fread(xi[mu], sizeof(**xi), N, f_in);
            }
            sprintf(buf, DIRJMA2 P_TYPE _U __NIS__ _S, N);
            make_dir(buf);
            sprintf(buf + strlen(buf), __AIS__ _U __TIS__ _U "%" PRIu32 EXTBIN,
                    Al, Ti_vals[kTi], m);
            if ((F_EEXIST(buf)))
            { //|| (1./T < beta_cH2((double) Ti_vals[kTi]/N))
                fprintf(f_log, MSGINFO PICJMNK __TIS__, Ti_vals[kTi]);
                continue;
            }
            if ((f_out = fopen(buf, "wb")) == NULL)
            {
                fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
                exit(EXIT_FAILURE);
            }
            /* write only the lower triangular matrix J_ij */
            for (uint32_t i = 0; i < N; i++)
            {
                for (uint32_t j = 0; j < i; j++)
                {
                    tmp_i32 = 0;
                    for (uint32_t mu = K * m; mu < K * (m + 1); mu++)
                        tmp_i32 += xi[mu][i] * xi[mu][j];
                    fwrite(&tmp_i32, sizeof(tmp_i32), 1, f_out);
                }
            }
            fclose(f_in);
            fclose(f_out);
            fprintf(f_log, MSGINFO PICJMOK __TIS__, Ti_vals[kTi]);
            tmp = KNUM;
            while (tmp)
                free(xi[--tmp]);
            free(xi);
        }
    }
    free(Ti_vals);
    fclose(f_log);
}
// do we really need to implement this? since it is already in main...
/* check pflip or tauint -- TO BE DONE*/
// sprintf(buf, "%s%sCHECK_T=%g.txt", dir_chck, P_TYPE, T);
// if ((f_in = fopen(buf, "r+")) == NULL) {
//     printf("%s%s%s%s", MSGFAIL, PFFOPEN, buf, MSGEXIT);
//     exit(EXIT_FAILURE);
// }
// double pflip;
// fscanf(f_in, "%lf", &pflip);
// if (pflip < PTHRESH) {
//     fprintf(f_log, "%s%s%.4lf%s", msg_warn, pw_markp, PTHRESH, MSGEXIT);
//     exit(EXIT_SUCCESS);
//     fclose(f_in);
//     fclose(f_log);
// } else fclose(f_in);