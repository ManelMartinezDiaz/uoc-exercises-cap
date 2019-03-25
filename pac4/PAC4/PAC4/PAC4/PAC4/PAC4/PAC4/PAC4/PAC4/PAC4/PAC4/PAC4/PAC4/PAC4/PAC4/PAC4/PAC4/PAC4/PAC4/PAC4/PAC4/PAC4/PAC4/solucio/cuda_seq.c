#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void initData(void);
void initValues(void);
void initValuesLin(void);

void calculateMovingAverage(void);
void calculateMovingAverageCuda(void);
void kernelMovingAverage(double *MOV_LIN_D, double *DATA_LIN_D, int blockIdx, int threadIdx);

void calculateColumnAverage(void);
void calculateColumnAverageCuda(void);
void kernelColumnAverage(double *AVG_D, double *DATA_LIN_D, int threadIdx);

void showData(void);
void showMov(void);
void showColAverage(void);
void showMovCuda(void);
void showColAverageCuda(void);

void checkResults(void);

void freeAll(void);

double **DATA;
double **MOV;

double *DATA_LIN;
double *DATA_TRANSPOSED_LIN;
double *MOV_LIN;

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

	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLUMNS; x++) {
	        MOV[y][x] = DATA[y][x];
	        for(i = 1; i <= 8; i++) {
	            if ((x - i) >= 0) {MOV[y][x] = MOV[y][x] + DATA[y][x - i];}
	        }
	        MOV[y][x] = MOV[y][x] / ELEMS_AVG;
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

void kernelMovingAverage(double *MOV_LIN_D, double *DATA_LIN_D, int blockIdx, int threadIdx) {
	int i;
	int posIni;
	double cell;

	cell = 0.0;

	posIni = blockIdx * COLUMNS + threadIdx;
	cell = DATA_LIN_D[posIni];

    for(i = 1; i <= 8; i++) {
        if ((threadIdx - i) >= 0) {cell = cell + DATA_LIN_D[posIni - i];}
    }

	cell = cell / ELEMS_AVG;

	MOV_LIN_D[posIni] = cell;
}

void calculateMovingAverageCuda() {
	int x, y;

	double *DATA_LIN_D;
	double *MOV_LIN_D;


	DATA_LIN_D = (double *) malloc(TOTAL_CELLS * sizeof(double));
	MOV_LIN_D = (double *) malloc(TOTAL_CELLS * sizeof(double));


	memset(MOV_LIN_D, 0, TOTAL_CELLS * sizeof(double));

	/* cudaMemCpy hot-to-device */
	memcpy(DATA_LIN_D, DATA_LIN, TOTAL_CELLS * sizeof(double));

    /* invoke kernel */
	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLUMNS; x++) {
			kernelMovingAverage(MOV_LIN_D, DATA_LIN_D, y, x);
		}
	}

	/* cudaMemCpy device-to-host */
	memcpy(MOV_LIN, MOV_LIN_D, TOTAL_CELLS * sizeof(double));

	free(MOV_LIN_D);
	free(DATA_LIN_D);
}

void kernelColumnAverage(double *AVG_D, double *DATA_LIN_D, int threadIdx) {
	int i;
	int posIni;
	int posFin;
	double cell;

	cell = 0.0;

	posIni = threadIdx * ROWS;
	posFin = posIni + ROWS;


	for (i = posIni; i < posFin; i++) {
		cell = cell + DATA_LIN_D[i];
	}

	cell = cell / ROWS;

	AVG_D[threadIdx] = cell;
}

void calculateColumnAverageCuda(void) {
	int x;

	double *DATA_TRANSPOSED_LIN_D;
	double *AVG_D;


	DATA_TRANSPOSED_LIN_D = (double *) malloc(TOTAL_CELLS * sizeof(double));

	AVG_D = (double *) malloc(COLUMNS * sizeof(double));


	memset(AVG_D, 0, COLUMNS * sizeof(double));

	/* cudaMemCpy hot-to-device */
	memcpy(DATA_TRANSPOSED_LIN_D, DATA_TRANSPOSED_LIN, TOTAL_CELLS * sizeof(double));

    /* invoke kernel */
	for (x = 0; x < COLUMNS; x++) {
	    kernelColumnAverage(AVG_D, DATA_TRANSPOSED_LIN_D, x);
	}

	/* cudaMemCpy device-to-host */
	memcpy(AVG_CUDA, AVG_D, COLUMNS * sizeof(double));

	free(DATA_TRANSPOSED_LIN_D);
	free(AVG_D);
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

	printf("Check result moving average : %d\n\n", result);

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

int main(int argc, char**argv) {
	if (argc != 3) {
		printf("usage: exercici_cuda N M\n N : number of rows\n M number of columns\n\n");
	    ROWS = 5;
	    COLUMNS = 10;
	} else {
	    ROWS = atoi(argv[1]);
	    COLUMNS = atoi(argv[2]);
	}

	ELEMS_AVG = 9;
	TOTAL_CELLS = ROWS * COLUMNS;

	initData();

	showData();

	calculateMovingAverage();
	showMov();

	calculateColumnAverage();
	showColAverage();

	calculateMovingAverageCuda();
	showMovCuda();

	calculateColumnAverageCuda();
	showColAverageCuda();

	checkResults();

	freeAll();
}
