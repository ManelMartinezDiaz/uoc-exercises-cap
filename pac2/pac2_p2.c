#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	int a[]={10,10,10,10,10,10,10,10,10,10};
	int N=10;
	int result=0, i=0;
	int tid;

	#pragma omp parallel for private(tid, i) reduction (+:result) shared(a)
		for(i=0; i<N; i++)
		{
		result = result + a[i];
		tid = omp_get_thread_num();
		printf("Thread number = %d. Resultat parcial = %d\n",tid, result);
		}
	printf("Resultat final = %d\n",result);	
}
