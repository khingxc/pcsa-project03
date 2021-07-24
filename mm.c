#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <string.h>
#include "mm.h"

// Task 1: Flush the cache so that we can do our measurement :)
void flush_all_caches(){

    for (long i = 0; i < (row * col); i++){
		asm volatile("clflush (%0)\n\t"
					: 
					: "r"(huge_matrixA + i)
					: "memory");
		asm volatile("clflush (%0)\n\t"
					: 
					: "r"(huge_matrixB + i)
					: "memory");
		asm volatile("clflush (%0)\n\t"
					: 
					: "r"(huge_matrixC + i)
					: "memory");
    }

    asm volatile("sfence\n\t" 
                 :
                 :
                 : "memory");

}

void load_matrix_base()
{
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
		huge_matrixC[i] = 0;		
	}
}

void free_all()
{
	free(huge_matrixA);
	free(huge_matrixB);
	free(huge_matrixC);
}

void multiply_base()
{
	// Your code here
	//
	// Implement your baseline matrix multiply here.

	for (long i = 0; i < row; i++){
		for (long v = 0; v < col; v++){
			long result = 0;
			for (long d = 0; d < col; d++){
				result += (huge_matrixA[(row * i) + d] * huge_matrixB[(d * col) + col]);
            }
            huge_matrixC[(i * row) + v] = result;
		}
	}

}

void compare_results()
{
	fout = fopen("./out.in","r");
	long i;
	long temp1, temp2;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fout, "%ld", &temp1);
		fscanf(fout, "%ld", &temp2);
		if(temp1!=temp2)
		{
			printf("Wrong solution!");
			exit(1);
		}
	}
	fclose(fout);
	fclose(ftest);
}

void write_results()
{
	// Your code here
	//
	// Basically, make sure the result is written on fout
	// Each line represent value in the X-dimension of your matrix

	char buffer[MAXBUF];
	fout = fopen("./out.in","w");
	for (long i = 0; i < row; i++){
		for (long j = 0; i < col; j++){
			memset(buffer, 0, MAXBUF);
			sprintf(buffer, "%ld", huge_matrixC[(i * row) + j]);
			fwrite(buffer, sizeof(char), strlen(buffer), fout);
		}
		fwrite("\n", sizeof(char), 2, fout);
	}
}

void load_matrix()
{
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)row*(long)col);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
		huge_matrixC[i] = 0;		
	}
}

void multiply()
{
	// Your code here
	//blocked matrix multiplication -> seperated blocks, done each and connected
	for (long ii = 0; ii < row; ii += BLOCK_SIZE){
		for (long jj = 0; jj < col; jj += BLOCK_SIZE){
			for (long kk = 0; kk < row; kk += BLOCK_SIZE){

				for (long i = 0; i < BLOCK_SIZE; i++){
					for (long j = 0; j < BLOCK_SIZE; j++){
						for (long k = 0; k < BLOCK_SIZE; k++){
							huge_matrixC[(row * (i+ii)) + (j+jj)] += (huge_matrixA[(row * (k+kk)) + (j+jj)] * huge_matrixB[(row * (i+ii)) + (k+kk)]);
						}
					}
				}
			}
		}
	}
}

int main()
{
	clock_t s,t;
	double total_in_base = 0.0;
	double total_in_your = 0.0;
	double total_mul_base = 0.0;
	double total_mul_your = 0.0;
	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");
	
	flush_all_caches();

	s = clock();
	load_matrix_base();
	t = clock();
	total_in_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the load = %f seconds\n", total_in_base);

	s = clock();
	multiply_base();
	t = clock();
	total_mul_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the multiply = %f seconds\n", total_mul_base);
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	free_all();

	flush_all_caches();

	s = clock();
	load_matrix();
	t = clock();
	total_in_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the load = %f seconds\n", total_in_your);

	s = clock();
	multiply();
	t = clock();
	total_mul_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the multiply = %f seconds\n", total_mul_your);
	write_results();
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	free_all();
	compare_results();

	return 0;
}