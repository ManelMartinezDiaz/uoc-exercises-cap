#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


void initData(void);
void initValues(void);
void initValuesLin(void);

void calculateMovingAverage(void);
void calculateMovingAverageCuda(void);
__global__ void kernelMovingAverage(double *MOV_LIN_D, double *DATA_LIN_D, int *ROWS);

void calculateColumnAverage(void);
void calculateColumnAverageCuda(void);
__global__ void kernelColumnAverage(double *AVG_D, double *DATA_LIN_D, int *COLUMNS, int *ELEMS_AVG);

void showData(void);
void showMov(void);
void showColAverage(void);
void showMovCuda(void);
void showColAverageCuda(void);

void createFilesData(void);
void createFilesMov(void);
void createFilesAvg(void);


void checkResults(void);

void freeAll(void);

double **DATA;
double **MOV;

double *DATA_LIN;
double *DATA_TRANSPOSED_LIN;
double *MOV_LIN;

double *DATA_TRANSPOSED_LIN_D;
double *AVG_D;

double *AVG;
double *AVG_CUDA;

const int OK = 1;
const int KO = 0;

int COLUMNS;
int ROWS;
int ELEMS_AVG;
int TOTAL_CELLS;

void initData(void) {
	int i;

	AVG = (double *) malloc(COLUMNS * sizeof(double));
	AVG_CUDA = (double *) malloc(COLUMNS * sizeof(double));
	DATA = (double **) malloc(ROWS * sizeof(double *));
	MOV = (double **) malloc(ROWS * sizeof(double *));
	DATA_LIN  = (double *) malloc(TOTAL_CELLS * sizeof(double));
	DATA_TRANSPOSED_LIN = (double *) malloc(TOTAL_CELLS * sizeof(double));
	MOV_LIN = (double *) malloc(TOTAL_CELLS * sizeof(double));

	for (i = 0; i < ROWS; i++) {
		DATA[i] = (double *) malloc(COLUMNS * sizeof(double));
		MOV[i] = (double *) malloc(COLUMNS * sizeof(double *));
	}

	initValues();
}

void initValues(void) {
	int i, j;

	srand(time(NULL));

	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLUMNS; j++) {
			DATA[i][j] = (100.0 * rand()) / ((double) RAND_MAX);
			MOV[i][j] = 0.0;
		}
	}

	for (i = 0; i < COLUMNS; i++) {
		AVG[i] = 0.0;
		AVG_CUDA[i] = 0.0;
	}

	initValuesLin();
}

void initValuesLin(void) {
	int i, j, k, l;

	srand(time(NULL));

	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLUMNS; j++) {
			k = i * COLUMNS + j;
			l = j * ROWS + i;
			DATA_LIN[k] = DATA[i][j];
			MOV_LIN[k] = MOV[i][j];
			DATA_TRANSPOSED_LIN[l] = DATA[i][j];
		}
	}
}

int canShow() {
	if ((ROWS <= 15) && (COLUMNS <= 15)) {
		return 1;
	} else {
		return 0;
	}
}

void showData(void) {
	int x, y;

	if (!canShow()) return;

	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLUMNS; x++) {
			printf("%07.4f ", DATA[y][x]);
		}
		printf("\n");
	}
	printf("\n");
}

void showMov(void) {
	int x, y;

	if (!canShow()) return;

	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLUMNS; x++) {
			printf("%07.4f ", MOV[y][x]);
		}
		printf("\n");
	}
	printf("\n");
}

void showMovCuda(void)  {
	int x, y;

	if (!canShow()) return;

	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLUMNS; x++) {
			printf("%07.4f ", MOV_LIN[y * COLUMNS + x]);
		}
		printf("\n");
	}
	printf("\n");

}

void showColAverage(void) {
	int x;

	if (!canShow()) return;

	for (x = 0; x < COLUMNS; x++) {
		printf("%07.4f ", AVG[x]);
	}
	printf("\n\n");
}

void showColAverageCuda(void) {
	int x;

	if (!canShow()) return;

	for (x = 0; x < COLUMNS; x++) {
		printf("%07.4f ", AVG_CUDA[x]);
	}
	printf("\n\n");
}

void calculateMovingAverage(void) {
        int x, y;
	int i;
	int elems; 

	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLUMNS; x++) {
	                MOV[y][x] = DATA[y][x];
		        elems = 1;
	                for(i = 1; i < ELEMS_AVG; i++) {
	                        if ((x - i) >= 0) {
		                        MOV[y][x] = MOV[y][x] + DATA[y][x - i];
			                elems++;
		                }
	                }
                        MOV[y][x] = MOV[y][x] / elems;
		}
	}
}

void calculateColumnAverage(void) {
    int x, y;

	for (x = 0; x < COLUMNS; x++) {
		for (y = 0; y < ROWS; y++) {
			AVG[x] = AVG[x] + DATA[y][x];
		}
	    AVG[x] = AVG[x] / ROWS;
	}
}

__global__ void kernelMovingAverage(double *MOV_LIN_D, double *DATA_LIN_D, int *COLUMNS, int *ELEMS_AVG) {
	int i;
	int posIni;
	double cell;
        int columns;
        int elemsAvg;
	int elems;
        int row;
        int column;

        columns = *COLUMNS;
        elemsAvg = *ELEMS_AVG;
        row = blockIdx.x;
        column = threadIdx.x;

	cell = 0.0;
        elems = 1;

	posIni = row * columns + column;
	cell = DATA_LIN_D[posIni];

        for(i = 1; i < elemsAvg; i++) {
            if ((column - i) >= 0) {
                cell = cell + DATA_LIN_D[posIni - i];
		elems++;
            }
        }
	
        cell = cell / elems;

	MOV_LIN_D[posIni] = cell;
}

void calculateMovingAverageCuda() {
        double *DATA_LIN_D;
        double *MOV_LIN_D;
	int *COLUMNS_D;
        int *ELEMS_AVG_D;

        cudaMalloc((void **) &DATA_LIN_D, TOTAL_CELLS * sizeof(double));
	cudaMalloc((void **) &MOV_LIN_D, TOTAL_CELLS * sizeof(double));
        cudaMalloc((void **) &COLUMNS_D, sizeof(int));
        cudaMalloc((void **) &ELEMS_AVG_D, sizeof(int));

	cudaMemcpy(DATA_LIN_D, DATA_LIN, TOTAL_CELLS * sizeof(double), cudaMemcpyHostToDevice );
        cudaMemcpy(MOV_LIN_D, MOV_LIN, TOTAL_CELLS * sizeof(double), cudaMemcpyHostToDevice );
        cudaMemcpy(COLUMNS_D, &COLUMNS, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(ELEMS_AVG_D, &ELEMS_AVG, sizeof(int), cudaMemcpyHostToDevice);

        dim3 numColumns(COLUMNS, 1);
        dim3 numRows(ROWS, 1);
        kernelMovingAverage<<<numRows, numColumns>>>(MOV_LIN_D, DATA_LIN_D, COLUMNS_D, ELEMS_AVG_D);

	cudaMemcpy(MOV_LIN, MOV_LIN_D, TOTAL_CELLS * sizeof(double), cudaMemcpyDeviceToHost);

        cudaFree(COLUMNS_D);
        cudaFree(ELEMS_AVG_D);
	cudaFree(MOV_LIN_D);
	cudaFree(DATA_LIN_D);
}

__global__ void kernelColumnAverage(double *AVG_D, double *DATA_LIN_D, int *ROWS) {
	int i;
	int posIni;
	int posFin;
        int rows;
	double cell;

        rows = *ROWS;

	cell = 0.0;

	posIni = threadIdx.x * rows;
	posFin = posIni + rows;

	for (i = posIni; i < posFin; i++) {
		cell = cell + DATA_LIN_D[i];
	}

	cell = cell / rows;

	AVG_D[threadIdx.x] = cell;
}

void calculateColumnAverageCuda(void) {
        double *DATA_TRANSPOSED_LIN_D;
        double *AVG_D;
        int *ROWS_D;

	cudaMalloc((void **) &DATA_TRANSPOSED_LIN_D, TOTAL_CELLS * sizeof(double));
	cudaMalloc((void **) &AVG_D, COLUMNS * sizeof(double));
        cudaMalloc((void **) &ROWS_D, sizeof(int));

	cudaMemcpy(DATA_TRANSPOSED_LIN_D, DATA_TRANSPOSED_LIN, TOTAL_CELLS * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(AVG_D, AVG, COLUMNS * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(ROWS_D, &ROWS, sizeof(int), cudaMemcpyHostToDevice);

        dim3 numColumns(COLUMNS, 1);
        dim3 numRows(1, 1);
	kernelColumnAverage<<<numRows, numColumns>>>(AVG_D, DATA_TRANSPOSED_LIN_D, ROWS_D);

	cudaMemcpy(AVG_CUDA, AVG_D, COLUMNS * sizeof(double), cudaMemcpyDeviceToHost);

        cudaFree(ROWS_D);
	cudaFree(DATA_TRANSPOSED_LIN_D);
	cudaFree(AVG_D);
}

void  checkResults(void) {
	int y,x, k;
	double diff;
	double epsilon;
	int result;

	result= OK;
	epsilon = 0.0000001;

	for(y = 0; y < ROWS; y++) {
		for(x = 0; x < COLUMNS; x++) {
			k = y * COLUMNS + x;
			diff = abs(MOV[y][x] - MOV_LIN[k]);

			if (diff > epsilon) {
				result = KO;
				break;
			}
		}
	}

	printf("\nCheck result moving average : %d\n\n", result);

	result= OK;

	for(x = 0; x < COLUMNS; x++) {
	    diff = abs(AVG[x] - AVG_CUDA[x]);

	    if (diff > epsilon) {
		    result = KO;
			break;
		}
	}

	printf("Check result column average : %d\n\n", result);
}


void freeAll(void) {
	int y;

	free(AVG);
	for (y = 0; y < ROWS; y++) {
		free(DATA[y]);
		free(MOV[y]);
	}

	free(DATA);
	free(MOV);
}

void createFilesData(void) {
	int i, j, k;
	char line[1024];
	char fileName[] = "DATA_0.txt";

	k = 0;
	FILE *fd;
	for (i = 0; i < ROWS; i++) {
		fileName[5] = '1' + i;
		fd = fopen(fileName, "w");
		for (j = 0; j < COLUMNS; j++) {
			fgets(line, sizeof(line), fd );
			k = i * COLUMNS + j;
			sprintf(line, "%f\n", DATA_LIN[k]);
			fputs(line, fd);
		}
		fclose(fd);
	}
}

void createFilesMov(void)  {
	int i, j, k;
	char line[1024];
	char fileName[] = "MOV_0.txt";

	k = 0;
	FILE *fd;
	for (i = 0; i < ROWS; i++) {
		fileName[4] = '1' + i;
		fd = fopen(fileName, "w");
		for (j = 0; j < COLUMNS; j++) {
			k = i * COLUMNS + j;
			sprintf(line, "%f\n", MOV_LIN[k]);
			fputs(line, fd);
		}
		fclose(fd);
	}
}

void createFilesAvg(void)  {
	int i;
	char line[1024];
	char fileName[] = "AVG.txt";

	FILE *fd;

	fd = fopen(fileName, "w");

	for (i = 0; i < COLUMNS; i++) {
		sprintf(line, "%f\n", AVG_CUDA[i]);
		fputs(line, fd);
	}

        fclose(fd);	
}

int main(int argc, char**argv) {
	if (argc != 4) {
		printf("usage: exercici_cuda elems_avg N M\n elems_avg : number of elements to consider in moving average calculations\n N : number of rows\n M number of columns\n\n");
		ELEMS_AVG = 9;
	        ROWS = 5;
	        COLUMNS = 10;
	} else {
		ELEMS_AVG = atoi(argv[1]);
	        ROWS = atoi(argv[2]);
	        COLUMNS = atoi(argv[3]);
	}

	TOTAL_CELLS = ROWS * COLUMNS;

	printf("begin\n");

	initData();

	showData();

	createFilesData();
	
	calculateMovingAverage();
	showMov();

	calculateColumnAverage();
	showColAverage();

	calculateMovingAverageCuda();
	showMovCuda();

	createFilesMov();

	calculateColumnAverageCuda();
	showColAverageCuda();

	createFilesAvg();
	
	checkResults();

	freeAll();

	printf("end\n");	
}
