//solver_ex7 N ITERS--> on N es la mida de la matriu NxN i ITERS les iteracions
//solver_ex7 100 20

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "mpi.h"

//#define N 100
//#define ITERS 20
// N and ITERS might be input arguments

double ** A;
int size, MyProc;
int tag = 1;
MPI_Status status;

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

void solve(double **A, int n, int iters)
{
   double diff, tmp;
   int i,j;
   int for_iters;
   int numRowProc, firstRow, lastRow;
   int N = n;
   int ITERS = iters;

   //Es calcula el número de files a repartir entre els procesos
   numRowProc = (int) (N+2) / size;

   //Es calcula quina serà la primera fila i la última per a cada procés
   firstRow = numRowProc * MyProc; //el número de files per cada procés multiplicat pel número del procés actual
   lastRow = firstRow + numRowProc; //la primera fila del procés actual més les files que s'encarrega cada proces 

   // Com la formula per calcula C necessita de la fila anterior i la seguent, ens trobem que al primer procés (0)
   // i a l'últim procés necessiten una fila anterior i una posterior, respectivament, per tant es defineixen 
   // matrius (N+2)*(N+2). Llavors al procés 0, assignem la primera fila al valor 1 i a l'últim procés assignem
   // a l'última fila el valor de N+1
   if (MyProc == 0 ) firstRow = 1;
   if (MyProc == size -1) lastRow = N+1;


   for (for_iters=0;for_iters<ITERS;for_iters++) 
   { 
     diff = 0.0;

     // Enviarem i rebrem les dades entre els processos per tal de dispossar de les dades per fer els calculs
     // Per evitar deadlocks al procés 0 primer enviarem i a la resta de processo primer rebrem

     // Si el procés actual es el primer (0):
     // 1.Envío la última del procés actual al procés següent
     // 2.Espera a rebre la primera fila del procés següent com la fila última+1 de l'actual procés

     if (MyProc == 0) {
        MPI_Send(A[lastRow], N+2, MPI_DOUBLE, MyProc+1, tag, MPI_COMM_WORLD);
        MPI_Recv(A[lastRow+1], N+2, MPI_DOUBLE, MyProc+1, tag, MPI_COMM_WORLD,&status);
//printf("\nProc # %d/%d envia fila %d  a %d i rep de %d\n", MyProc, size, lastRow, MyProc+1, MyProc+1);

     } 
     // Si el procés actual es l'últim:
     // 1.Espera a rebre la última fila del procés anterior, com la fila-1 del procés actual
     // 2.Envía la primera fila del procés actual al procés anterior
     else if (MyProc == size-1){
        MPI_Recv(A[firstRow-1], N+2, MPI_DOUBLE, MyProc-1, tag, MPI_COMM_WORLD,&status);
        MPI_Send(A[firstRow], N+2, MPI_DOUBLE, MyProc-1, tag, MPI_COMM_WORLD);
//printf("\nProc # %d/%d envia fila %d  a %d i rep de %d\n", MyProc, size, lastRow, MyProc+1, MyProc+1);
     } else {
     // Si el procés es diferent a 0 o l'últim:
     // 1.Espera a rebre la última fila del procés anterior, com la fila-1 del procés actual
     // 2.Envía la primera fila del procés actual al procés anterior
     // 3.Envía la última fila del procés actual al procés següent
     // 4.Espera a rebre la primera fila del procés següent, com la última+1 de l'actual procés
        MPI_Recv(A[firstRow-1], N+2, MPI_DOUBLE, MyProc-1, tag, MPI_COMM_WORLD,&status);
        MPI_Send(A[firstRow], N+2, MPI_DOUBLE, MyProc-1, tag, MPI_COMM_WORLD);
        MPI_Send(A[lastRow], N+2, MPI_DOUBLE, MyProc+1, tag, MPI_COMM_WORLD);
        MPI_Recv(A[lastRow+1], N+2, MPI_DOUBLE, MyProc+1, tag, MPI_COMM_WORLD,&status);
//printf("\nProc # %d/%d envia fila %d a %d i rep fila %d de %d\n", MyProc, size, firstRow, MyProc+1, lastRow, MyProc+1);

     }

    for(i=firstRow;i<lastRow;i++)
       {
	for (j=1;j<n;j++)
	{
         tmp = A[i][j];
         A[i][j] = 0.3*(A[i][j] + A[i][j-1] + A[i-1][j] + A[i][j+1] + A[i+1][j]);
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
  char hostname[256];

  int N = atoi(argv[1]); // Mida matriu
  int ITERS = atoi(argv[2]); // Número iteracions 
  A = malloc((N+2) * sizeof(double *));
  for (i=0; i<N+2; i++) {
    A[i] = malloc((N+2) * sizeof(double)); 
  }

  initialize(A,N);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &MyProc);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  gethostname(hostname,255);

//  printf("\nProc # %d/%d iniciat en %s\n", MyProc, size, hostname);

  t_start=usecs();
  solve(A, N, ITERS);
  t_end=usecs();
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  
  time = ((double)(t_end-t_start))/1000000;
  printf("Temps de computació = %d [sg].\n", time);

  return 0;

}


