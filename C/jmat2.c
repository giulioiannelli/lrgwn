#include "head/dhnnlib.h"

#define NARG 1 + 2
#define PROGN NJCO2

#define LSEED1 0xFBCAFAE
#define LSEED2 0x187CCDD

int main(int argc, char *argv[])
{
    /* check argc - make logfile - initialize the sfmt rng */
    check_argc(argc, NARG);
    sprintf(buf, DIRLOGF PROGN __NIS "%s" _U __TIS "%s" EXTLOG,
            argv[1], argv[2]);
    make_log(argc, buf, PROGN, argv);
    /* initialize the sfmt random number generator */
    seed_rand[2] = LSEED1;
    seed_rand[3] = LSEED2;
    sfmt_init_by_array(&sfmt, seed_rand, LENSRND);
    /* variable declaration - acquisition from command line */
    FILE *f_in, *f_out;
    signed char **xi;
    int32_t tmp_i32;
    uint32_t tmp, K;
    uint32_t patset_m, patset_M;
    uint32_t alen = 0, Kcum = 0;
    uint64_t effKNUM;
    double Al;
    double *a_vals;
    N = strtoul_chck(argv[1], 10);
    T = strtod_chck(argv[2]);
    /* find the file, if exists, */
    find_CFGS();
    sprintf(buf2, DIRCFGS P_TYPE _U __NIS__ _U __TIS__ _U "[", N, T);
    strncpy(buf3, buf + strlen(buf2), strlen(buf) - strlen(buf2) - 5);
    effKNUM = strtoull_chck(buf3, 10);
    /* initialize Klist */
    sprintf(buf, DIRINPT KPFX _S __NIS__ _U __TIS__ EXTTXT, T, N);
    if ((f_in = fopen(buf, "r+")) == NULL)
    {
        fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    } /* qui si potrebbe pensare a fare un piccolo listato che se non trova il file lo crea... infondo basta eseguire il programma py*/
    a_vals = malloc(sizeof(*a_vals));
    challoc(a_vals);
    while (fscanf(f_in, "%lf", &Al) != EOF)
    {
        K = (uint32_t)(Al * N);
        a_vals[alen] = Al;
        Kcum += K;
        alen++;
        tmpntr = realloc(a_vals, sizeof(*a_vals) * (alen + 1));
        challoc(tmpntr);
        a_vals = tmpntr;
    }
    fclose(f_in);
    if (Kcum * AVG_M > effKNUM)
    {
        fprintf(f_log, MSGFAIL PFNKNUM MSGEXIT);
        exit(EXIT_FAILURE);
    }
    /* initialize patterns */
    sprintf(buf, DIRCFGS P_TYPE _U __NIS__ _U __TIS__ _U __KNUMIS__ EXTBIN,
            N, T, effKNUM);
    if ((f_in = fopen(buf, "rb")) == NULL)
    {
        fprintf(f_log, MSGFAIL PFFOPEN "%s" MSGEXIT, buf);
        exit(EXIT_FAILURE);
    }
    xi = malloc(sizeof(*xi) * effKNUM);
    challoc(xi);
    for (uint32_t mu = 0; mu < effKNUM; mu++)
    {
        xi[mu] = malloc(sizeof(**xi) * N);
        challoc(xi[mu]);
        fread(xi[mu], sizeof(**xi), N, f_in);
    }
    fclose(f_in);
    /* generate coupling matrix */
    for (uint16_t m = 0; m < AVG_M; m++)
    {
        for (uint32_t ap = 0; ap < alen; ap++)
        {
            K = (uint32_t)(a_vals[ap] * N);
            patset_m = K * m;
            patset_M = K * (m + 1);
            sprintf(buf, DIRJMA2 P_TYPE _U __NIS__ _S, N);
            make_dir(buf);
            sprintf(buf + strlen(buf), __TIS__ _U __KIS__ _U "%" PRIu16 EXTBIN,
                    T, K, m);
            if ((F_EEXIST(buf)))
            { //|| (1./T < beta_cH2((double) K_vals[kp]/N))
                fprintf(f_log, MSGINFO PICJMNK __AIS__, a_vals[ap]);
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
                    for (uint32_t mu = patset_m; mu < patset_M; mu++)
                        tmp_i32 += xi[mu][i] * xi[mu][j];
                    fwrite(&tmp_i32, sizeof(tmp_i32), 1, f_out);
                }
            }
            fclose(f_out);
            fprintf(f_log, MSGINFO PICJMOK __KIS__, K);
        }
    }
    tmp = effKNUM;
    while (tmp)
        free(xi[--tmp]);
    free(xi);
    free(a_vals);
    fclose(f_log);
}