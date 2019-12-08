// Robear Mankaryous rmankaryous@wpi.edu

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
char transpose_32_desc[] = "Transpose for a 32 x 32 matrix";
void transpose_32(int A[32][32], int B[32][32]) {
    int block_size = 8;
    int temp_diag = 0;
    int temp_place = 0;
    for (int col = 0; col < 32; col += block_size) {
	for (int row = 0; row < 32; row += block_size) {
	    for (int block_row = row; block_row < (row + block_size); block_row++) {
		for (int block_col = col; block_col < (col + block_size); block_col++) {
		    if (block_row != block_col) {
			B[block_col][block_row] = A[block_row][block_col];
		    }
		    else {
			temp_diag = A[block_row][block_col];
			temp_place = block_row;
		    }
		}
		if (row == col) {
		    B[temp_place][temp_place] = temp_diag;
		}
	    }
	}
    }
}

char transpose_64_desc[] = "Transpose for a 64 x 64 matrix";
void transpose_64(int A[64][64], int B[64][64]) {
    int block_size = 4;
    int temp_diag = 0;
    int temp_place = 0;
    for (int col = 0; col < 64; col += block_size) {
	for (int row = 0; row < 64; row += block_size) {
	    for (int block_row = row; block_row < (row + block_size); block_row++) {
		for (int block_col = col; block_col < (col + block_size); block_col++) {
		    if (block_row != block_col) {
			B[block_col][block_row] = A[block_row][block_col];
		    }
		    else {
			temp_diag = A[block_row][block_col];
			temp_place = block_row;
		    }
		}
		if (row == col) {
		    B[temp_place][temp_place] = temp_diag;
		}
	    }
	}
    }
}

char transpose_alt_desc[] = "Transpose for an alternative matrix";
void transpose_alt(int M, int N, int A[N][M], int B[M][N]) {
    int block_size = 16;
    int temp_diag = 0;
    int temp_place = 0;
    for (int col = 0; col < M; col += block_size) {
	for (int row = 0; row < N; row += block_size) {
	    for (int block_row = row; (block_row < (row + block_size)) && (block_row < N); block_row++) {
		for (int block_col = col; (block_col < (col + block_size)) && (block_col < M); block_col++) {
		    if (block_row != block_col) {
			B[block_col][block_row] = A[block_row][block_col];
		    }
		    else {
			temp_place = block_row;
			temp_diag = A[block_row][block_col];
		    }
		}
		if (row == col && temp_place < M && temp_place < N) {
		    B[temp_place][temp_place] = temp_diag;
		}
	    }
	}
    }
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M != N) {
	transpose_alt(M, N, A, B);
	return;
    }
    switch(M) {
    case 32:
	transpose_32(A, B);
	break;
    case 64:
	transpose_64(A, B);
	break;
    default:
	transpose_alt(M, N, A, B);
	break;
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

