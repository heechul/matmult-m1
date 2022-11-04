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

// #include "fast_float/fast_float.h"
#include "m1cycles.h"
#include <algorithm>
#include <charconv>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale.h>
#include <random>
#include <sstream>
#include <stdio.h>
#include <vector>

// #include "random_generators.h"

/* change dimension size as needed */
struct timeval tv; 
int dimension = 1024;


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



int main(int argc, char *argv[]) 
{
	double *A, *B, *C;
	int i, j;
	int opt;
	int algo = 0;


  	setup_performance_counters();

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

	printf("dimention = %d algo = %d\n", dimension, algo);

	A = (double*)malloc(dimension*dimension*sizeof(double));
	B = (double*)malloc(dimension*dimension*sizeof(double));
	C = (double*)malloc(dimension*dimension*sizeof(double));

	srand(292);

	// matrix initialization
	for(i = 0; i < dimension; i++) {
		for(j = 0; j < dimension; j++)
		{   
			A[dimension*i+j] = (rand()/(RAND_MAX + 1.0));
			B[dimension*i+j] = (rand()/(RAND_MAX + 1.0));
			C[dimension*i+j] = 0.0;
		}
	}

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
  	double start_ts, end_ts; /* time */
	// do matrix multiplication
	start_ts = timestamp();
  	performance_counters start = get_counters();
	switch(algo) {
		case 0:
			matmult(A, B, C, dimension);
			break;
		case 1:
			matmult_opt1(A, B, C, dimension);
			break;
	}
  	performance_counters end = get_counters();
  	performance_counters diff = end - start;

	end_ts = timestamp();
  
	// print sum
	double sum = 0; 
	for (i = 0; i < dimension; i++) {
		for (j = 0; j < dimension; j++) {
			sum += C[dimension * i + j];
		}
	}
	printf("sums: %f\n", sum);

	free(A);
	free(B);
	free(C);

	printf(" %18.2f nanosecs \n", (end_ts-start_ts)*1000000000);

	printf(" %18.2f instructions\n", diff.instructions);
	printf(" %18.2f cycles\n", diff.cycles);
	printf(" %18.2f dcache load misses \n", diff.branches);
	printf(" %18.2f dtlb misses \n", diff.missed_branches);

	return EXIT_SUCCESS;
}