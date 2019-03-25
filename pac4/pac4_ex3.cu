#include <stdio.h>
#define N 1000 // Nº Columnes
#define M 10  // Nº Files
#define ELE 4 // Elements anteriors

__global__ void moving_average(float *a, float *b) {

	int index = blockDim.x * blockIdx.x + threadIdx.x;
	float result;

	if (index % N >= ELE){ //El primer valor de cada fila a calcular és el Nº ELE
		for(int i=0;i < ELE;i++) {
		result = result + a[index-i];
		}
		b[index] = result/ELE;
	} else {
		b[index] = a[index];
	}
}

__global__ void average_col(float *a, float *b) {

	int index = blockDim.x * blockIdx.x + threadIdx.x;
	int column = threadIdx.x;  //  Tenim un thread per columna segons definit a DimBlock (ThreadsPerBlock)
	float result;

	// Recorrer els valors en vertical de la matriu DADES i fer mitjana
	// Per a cada columna (column = nº threads per simplificar) es suma el valor de totes les files (N*i)
	for(int i=0; i<M; i++){
		result = result + a[N*i+column];
	}

        b[index] = result/M;  // Es fa la mitjana amb el nº de files
	
}


int main()
{
	// Definim els punters de les matrius/vector (DADES, MOV i AVG) al host i device
	float *host_data, *host_mov, *host_avg;
	float *device_data, *device_mov, *device_avg;

	const int size_host_data = N*M*sizeof(float);
	const int size_host_mov = N*M*sizeof(float);
	const int size_host_avg = N*sizeof(float);

	// Assignem memòria per a les matrius/vector al host i device
	host_data = (float*)malloc(size_host_data);
	host_mov = (float*)malloc(size_host_mov);
	host_avg = (float*)malloc(size_host_avg);

	cudaMalloc((void**)&device_data, size_host_data);
	cudaMalloc((void**)&device_mov, size_host_mov);
	cudaMalloc((void**)&device_avg, size_host_avg);

        // Inicilitzem les matrius
	// Matriu DATA[N,M] --> host_data --> Valors aletaris del 0 al 10
	// Matriu MOV[N,M] --> host_mov --> '0'
	// Vector AVG[N] --> host_avg --> '0'
	int i, j;

	for(i=0;i<N*M;i++){ host_data[i]= (float)(rand() % 10);} 
        for(i=0;i<N*M;i++){ host_mov[i]= 0;}
        for(j=0;j<N;j++){ host_avg[j]= 0;}

	//
	printf("Resultat execució matriu DADES[%i,%i].\n", N, M);

        // Mostrar matriu DADES
        printf("Matriu DADES[N,M]:\n");
        for(int i=0;i<M;i++){
                printf("Row Nº%i |",i);
                for(int j=0;j<N;j++){
                        printf(" %f |",host_data[i*N+j]);
                }
                printf("\n");
        }

	// Copiem les matrius/vector del host al device
	cudaMemcpy( device_data, host_data, size_host_data, cudaMemcpyHostToDevice);
	cudaMemcpy( device_mov, host_mov, size_host_mov, cudaMemcpyHostToDevice);
	cudaMemcpy( device_avg, host_avg, size_host_avg, cudaMemcpyHostToDevice);
	
	// Definim el nº de blocs, el nº de fluxos i s'invoca el kernel per calcular el 'Moving Average'
	// Creem un únic bloc i un thread per a cada 
        int blocksPerGrid = 1; // Nº de blocs
	int threadsPerBlock = N * M; // Nº de blocs
	moving_average<<<blocksPerGrid, threadsPerBlock>>>(device_data, device_mov);

	cudaMemcpy( host_mov, device_mov, size_host_mov, cudaMemcpyDeviceToHost );
	cudaFree( host_mov);

        // Definim el nº de blocs, el nº de fluxos i s'invoca el kernel per calcular el	'Average Column'
	// La matriu dades encara està al dispositiu
	// Creem un únic bloc i un thread per cada columna
        blocksPerGrid = 1;
        threadsPerBlock = N;
	average_col<<<blocksPerGrid, threadsPerBlock>>>(device_data,device_avg);
        cudaMemcpy( host_avg, device_avg, size_host_avg, cudaMemcpyDeviceToHost );
	cudaFree( host_data);
	cudaFree( host_avg);

        // Mostrar matriu MOV
        printf("Matriu MOV[N,M]:\n");
        for(int i=0;i<M;i++){
                printf("Row Nº%i |",i);
                for(int	j=0;j<N;j++){
                        printf(" %f |",host_mov[i*N+j]);
                }
                printf("\n");
        }

        // Mostrar vector AVG
        printf("Matriu AVG[N]:\n");
	printf("Average: ");
        for(int i=0;i<N;i++){
        	printf(" %f |",host_avg[i]);
        }
        printf("\n");

return EXIT_SUCCESS; 
}
