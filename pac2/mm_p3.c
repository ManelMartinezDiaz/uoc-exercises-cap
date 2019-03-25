#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <memory.h>
#include <malloc.h>
#include <omp.h>

int main(int argc, char **argv) {

  int SIZE = atoi(argv[1]);

  float matrixa[SIZE][SIZE], matrixb[SIZE][SIZE], mresult[SIZE][SIZE];
  int i,j,k;

/* Begin parallel region */

/*Initialize the Matrix arrays */
#pragma omp parallel private(i)
{
  /* Parallelize the initialization of Matrix arrays */
  /* Indice i is private for each trhead to iterate independly */
  #pragma omp parallel for schedule (runtime)
  for ( i=0; i<SIZE*SIZE; i++ ){
    mresult[0][i] = 0.0;}
  #pragma omp parallel for schedule (runtime)
  for ( i=0; i<SIZE*SIZE; i++ ){
    matrixa[0][i] = matrixb[0][i] = rand()*(float)1.1; }

}

/* Matrix-Matrix multiply */

#pragma omp parallel shared(matrixa, matrixb, mresult) private(i,j,k)
{

 /* Parallelize the outer loop which evaluates each row of matrix resultant */
 /* Matrices are shared among threads*/ 
 /* Indices are privates for each threads to iterate independly */
 /* I use static assuming all multiplication operations are equal time consuming */
  #pragma omp for schedule (runtime)
  for (i=0;i<SIZE;i++)
   for(j=0;j<SIZE;j++)
    for(k=0;k<SIZE;k++)
      mresult[i][j]=mresult[i][j] + matrixa[i][k]*matrixb[k][j];
}

/* End of parallel region */
  printf("Done.\n");
exit(0);
}
