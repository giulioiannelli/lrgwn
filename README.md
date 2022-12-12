## Balancing of Hebbian coupling matrices of Hopfield model with correlated patters
### _or_ The Laplacian Renormalization Group of weighted networks

Git clone the directory with submodules (i.e. the fast Mersenne Twister random number generator `SFMT`):
```
git clone --recurse-submodules https://github.com/giulioiannelli/lrgwn
```
Compile the C program for the generation of coupling matrices $J_{ij}$:
```
cd lrgwn
gcc -O3 -DSFMT_MEXP=19937 -o exe/Jxixi.o C/head/SFMT/SFMT.c C/Jxixi.c -Wall -lm
```
The name of the prgram output can be safely changed from `Jxixi` to whatever[^1].

To execute the `python` script run `python3 py/LaplSM_Jxixi.py`. Optional and mandatory arguments are listed when calling for help `python3 py/LaplSM_Jxixi.py -h`. 

The four mandatory arguments are, in the order, $N$, $\beta=1/T$, $\alpha = K/N$ and the number of averages ${\rm nAvg}$. E.g. `python3 py/LaplSM_Jxixi.py 4096 0.5 0.1 2` generates the file `res/tmp/N=4096/eigs_T=2_K=409.dat`, whose content is
```
# nAvg           T                K                sumE             sumEsq         
1.0000000000e+01 1.0000000000e+00 5.1200000000e+02 2.6005457573e+01 3.5601927174e+02
```



[^1]: This provided that in the `python` script one passes the option `--pname=<chosen-name>.o`
