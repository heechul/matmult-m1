// https://vaibhaw-vipul.medium.com/matrix-multiplication-optimizing-the-code-from-6-hours-to-1-sec-70889d33dcfa

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
// #include <omp.h>

/* change dimension size as needed */
struct timeval tv; 
int dimension = 1024;
double start, end; /* time */

double timestamp()
{
        double t;
        gettimeofday(&tv, NULL);
        t = tv.tv_sec + (tv.tv_usec/1000000.0);
        return t;
}

// A naive matrix multiplication implementation. 
void matmult(double *A, double *B, double *C, int dimension)
{
	for(int i = 0; i < dimension; i++) {
		for(int j = 0; j < dimension; j++) {
			for(int k = 0; k < dimension; k++) {
				C[dimension*i+j] += A[dimension*i+k] * B[dimension*k+j];
			}
		}
	}	
}

// a better cache optimized version: change the order to improve the cache hit rate
void matmult_opt1(double *A, double *B, double *C, int dimension)
{
	for(int i = 0; i < dimension; i++) {
		for(int k = 0; k < dimension; k++) {
			for(int j = 0; j < dimension; j++) {
				C[dimension*i+j] += A[dimension*i+k] * B[dimension*k+j];
			}
		}
	}	
}


// openmp
void matmult_opt2(double *A, double *B, double *C, int dimension)
{
	int i,j,k;
	#pragma omp parallel for private(i,j,k) shared(A,B,C)
	for(i = 0; i < dimension; i++) {
		for(k = 0; k < dimension; k++) {
			for(j = 0; j < dimension; j++) {
				C[dimension*i+j] += A[dimension*i+k] * B[dimension*k+j];
			}
		}
	}	
}

int main(int argc, char *argv[])
{
	double *A, *B, *C;
	unsigned finish = 0;
	int i, j, k;

	int opt;
	int algo = 0;

	/*
	 * get command line options 
	 */
	while ((opt = getopt(argc, argv, "m:a:n:t:c:i:p:o:f:l:xh")) != -1) {
		switch (opt) {
		case 'n':
			dimension = strtol(optarg, NULL, 0);
			break;
		case 'a':
			algo = strtol(optarg, NULL, 0);
			break;
		}
	}

	printf("dimension: %d, algorithm: %d\n", dimension, algo);

	A = (double*)malloc(dimension*dimension*sizeof(double));
	B = (double*)malloc(dimension*dimension*sizeof(double));
	C = (double*)malloc(dimension*dimension*sizeof(double));

	srand(292);

	// Matrix initialization
	for(i = 0; i < dimension; i++) {
		for(j = 0; j < dimension; j++)
		{   
			A[dimension*i+j] = (rand()/(RAND_MAX + 1.0));
			B[dimension*i+j] = (rand()/(RAND_MAX + 1.0));
			C[dimension*i+j] = 0.0;
		}
	}

	// do matrix multiplication
	start = timestamp();
	switch(algo) {
		case 0:
			matmult(A, B, C, dimension);
			break;
		case 1:
			matmult_opt1(A, B, C, dimension);
			break;
		case 2:
			matmult_opt2(A, B, C, dimension);
			break;
	}
	end = timestamp();

	// print sum
	double sum = 0; 
	for (i = 0; i < dimension; i++) {
		for (j = 0; j < dimension; j++) {
			sum += C[dimension * i + j];
		}
	}
	printf("secs: %.6f\n", end-start);
	printf("sums: %f\n", sum);
	free(A);
	free(B);
	free(C);

	return 0;
}
