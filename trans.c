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
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, ii, jj;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    if(M == 32) {
        for (i = 0; i < N; i+=8) {
            for (j = 0; j < M; j++) {
                tmp0 = A[i][j];
                tmp1 = A[i+1][j];
                tmp2 = A[i+2][j];
                tmp3 = A[i+3][j];
                tmp4 = A[i+4][j];
                tmp5 = A[i+5][j];
                tmp6 = A[i+6][j];
                tmp7 = A[i+7][j];
                
                B[j][i+0] = tmp0;
                B[j][i+1] = tmp1;
                B[j][i+2] = tmp2;
                B[j][i+3] = tmp3;
                B[j][i+4] = tmp4;
                B[j][i+5] = tmp5;
                B[j][i+6] = tmp6;
                B[j][i+7] = tmp7;
            }
        }
    } else if(M == 64) {
        for (ii = 0; ii < N; ii += 8) {
            for (jj = 0; jj < M; jj += 8) {
                if ((ii&0x3C) == (jj&0x3C)) {
                    for (i = ii; i < 8 + ii; i += 4) {
                        for (j = jj; j < 8 + jj; j += 2) {             
                            tmp0 = A[i+0][j];
                            tmp1 = A[i+1][j];
                            tmp2 = A[i+2][j];
                            tmp3 = A[i+3][j];
                            tmp4 = A[i+0][j+1];
                            tmp5 = A[i+1][j+1];
                            tmp6 = A[i+2][j+1];
                            tmp7 = A[i+3][j+1];

                            B[j][i+0] = tmp0;
                            B[j][i+1] = tmp1;
                            B[j][i+2] = tmp2;
                            B[j][i+3] = tmp3;
                            B[j+1][i+0] = tmp4;
                            B[j+1][i+1] = tmp5;
                            B[j+1][i+2] = tmp6;
                            B[j+1][i+3] = tmp7;
                        }
                    }
                }
                else {
                    for (i = ii; i < 4 + ii; i++) {
                        for (j = jj; j < 4 + jj; j++) {
                            B[j][i] = A[i][j];
                        }
                    }
                    tmp0 = A[ii][jj+4];
                    tmp1 = A[ii][jj+5];
                    tmp2 = A[ii][jj+6];
                    tmp3 = A[ii][jj+7];
                    tmp4 = A[ii+1][jj+4];
                    tmp5 = A[ii+1][jj+5];
                    tmp6 = A[ii+1][jj+6];
                    tmp7 = A[ii+1][jj+7];
                    for (i = ii + 4; i < ii + 8; i++) {
                        for (j = jj; j < jj + 4; j++) {
                            B[j][i] = A[i][j];
                        }
                    }
                    for (i = ii + 4; i < ii + 8; i++) {
                        for (j = jj + 4; j < jj + 8; j++) {
                            B[j][i] = A[i][j];
                        }
                    }
                    B[jj+4][ii] = tmp0;
                    B[jj+5][ii] = tmp1;
                    B[jj+6][ii] = tmp2;
                    B[jj+7][ii] = tmp3;
                    B[jj+4][ii+1] = tmp4;
                    B[jj+5][ii+1] = tmp5;
                    B[jj+6][ii+1] = tmp6;
                    B[jj+7][ii+1] = tmp7;
                    for (i = ii + 2; i < ii + 4; i++) {
                        for (j = jj + 4; j < jj + 8; j++) {
                            B[j][i] = A[i][j];
                        }
                    }
                }
            }
        }
    } else {
        for (j = 0; j < M; j+=8) {
            for (i = 0; i < N; i++) {
                tmp0 = A[i][j];
                tmp1 = A[i][j+1];
                tmp2 = A[i][j+2];
                tmp3 = A[i][j+3];
                tmp4 = A[i][j+4];
                tmp5 = A[i][j+5];
                tmp6 = A[i][j+6];
                tmp7 = A[i][j+7];
                B[j][i] = tmp0;
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
                B[j+4][i] = tmp4;
                B[j+5][i] = tmp5;
                B[j+6][i] = tmp6;
                B[j+7][i] = tmp7;
            }
        }
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

