#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <memory.h>
#include <malloc.h>
#include <papi.h>
#include <omp.h>

#define SIZE 1000

int main(int argc, char **argv) {

  float matrixa[SIZE][SIZE], matrixb[SIZE][SIZE], mresult[SIZE][SIZE];
  int i,j,k;
  int events[3] = {PAPI_L1_TCM, PAPI_L2_TCM,PAPI_L3_TCM}, ret;
  long long values[3];

  if (PAPI_num_counters() < 3) {
  	fprintf(stderr, "No hardware counters here, or PAPI not supported.\n");
  	exit(1);
  }

  if ((ret = PAPI_start_counters(events, 3)) != PAPI_OK) {
  	fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(ret));
  	exit(1);
  }

/* Begin parallel region */

  /* Initialize the Matrix arrays */
  #pragma omp parallel private(i)
  {
    #pragma omp sections nowait
    {
    #pragma omp section
    for ( i=0; i<SIZE*SIZE; i++ ){
      mresult[0][i] = 0.0;}
    #pragma omp section
    for ( i =0;i<SIZE*SIZE; i++ ){
      matrixa[0][i] = matrixb[0][i] = rand()*(float)1.1; }
    /* End of sections */
    }
  }
  /* End of parallel region */

  /* Matrix-Matrix multiply */
  #pragma omp parallel shared(matrixa, matrixb, mresult) private(i,j,k)
  {
    #pragma omp for
    for (i=0;i<SIZE;i++)
     for(j=0;j<SIZE;j++)
      for(k=0;k<SIZE;k++)
        mresult[i][j]=mresult[i][j] + matrixa[i][k]*matrixb[k][j];
  }
  /* End of parallel region */

  if ((ret = PAPI_read_counters(values, 3)) != PAPI_OK) {
  	fprintf(stderr, "PAPI failed to read counters: %s\n", PAPI_strerror(ret));
  	exit(1);
  }

  printf("Cache Misses Level1: %lld\n",values[0]);
  printf("Cache Misses Level2: %lld\n",values[1]);
  printf("Cache Misses Level3: %lld\n",values[2]);
  exit(0);
}
