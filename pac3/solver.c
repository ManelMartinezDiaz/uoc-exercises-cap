#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define N 1000
#define ITERS 100
// N and ITERS might be input arguments

double ** A;

void initialize (double **A, int n)
{
   int i,j;

   for (j=0;j<n+1;j++){
     A[0][j]=1.0;
   }
   for (i=1;i<n+1;i++){
      A[i][0]=1.0;
      for (j=1;j<n+1;j++) A[i][j]=0.0;
   }
}

void solve(double **A, int n)
{
   double diff, tmp;
   int i,j, iters=0;
   int for_iters;


   for (for_iters=0;for_iters<ITERS;for_iters++) 
   { 
     diff = 0.0;

     for (i=1;i<n;i++)
     {
       for (j=1;j<n;j++)
       {
         tmp = A[i][j];
         A[i][j] = 0.3*(A[I][j] + A[i][j-1] + A[i-1][j] + A[i][j+1] + A[i+1][j]);
         diff += fabs(A[i][j] - tmp);
         /*printf("partial dif is %f \n ", A[i][j] - tmp);*/
       }

     }
     iters++;

    } /*for*/
}


long usecs (void)
{
  struct timeval t;

  gettimeofday(&t,NULL);
  return t.tv_sec*1000000+t.tv_usec;
}


int main(int argc, char * argv[])
{
  int i;
  long t_start,t_end;
  double time;

  A = malloc((N+2) * sizeof(double *));
  for (i=0; i<N+2; i++) {
    A[i] = malloc((N+2) * sizeof(double)); 
  }

  initialize(A, N);

  t_start=usecs();
  solve(A, N);
  t_end=usecs();

  time = ((double)(t_end-t_start))/1000000;
  printf("Computation time = %f\n", time);

}
