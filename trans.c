//517030910116    Jingwei Xi
//email: jingweixi@sjtu.edu.cn
//This is the program for matrix transpose function
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
    int i, j, m, n;
    int a1, a2, a3, a4, a5, a6, a7, a8;
    if(N == 32 && M == 32){   // Matrix 32x32
        for(i = 0; i < 4; i++){
            for(j = 0; j < 4; j++){
                for(m = 0; m < 8; m++){
                    for(n = 0; n < 8; n++){
                        //For A[k][k], handle it later
                        if(i * 8 + m == j * 8 + n){
                            a1 = i * 8 + m;
                            a2 = A[i * 8 + m][j * 8 + n];
                            continue;
                        }
                        B[j * 8 + n][i * 8 + m] = A[i * 8 + m][j * 8 + n];
                    }
                    if(i == j){
                        //Handle the A[k][k]
                        B[a1][a1] = a2;  
                    }
                }
            }
        }
        return;
    }

    if(N == 64 && M == 64){  //Matrix 64x64
        
        for(i = 0; i < 8; i++){
            for(j = 0; j < 8; j++){
                //Transpose A,B to AT, BT
                for(m = 0; m < 4; m++){
                    a1 = A[i * 8 + m][j * 8]; 
                    a2 = A[i * 8 + m][j * 8 + 1];
                    a3 = A[i * 8 + m][j * 8 + 2]; 
                    a4 = A[i * 8 + m][j * 8 + 3];
                    a5 = A[i * 8 + m][j * 8 + 4]; 
                    a6 = A[i * 8 + m][j * 8 + 5];
                    a7 = A[i * 8 + m][j * 8 + 6]; 
                    a8 = A[i * 8 + m][j * 8 + 7];

                    B[j * 8][i * 8 + m] = a1;
                    B[j * 8][i * 8 + m + 4] = a5;
                    B[j * 8 + 1][i * 8 + m] = a2;
                    B[j * 8 + 1][i * 8 + m + 4] = a6;
                    B[j * 8 + 2][i * 8 + m] = a3;
                    B[j * 8 + 2][i * 8 + m + 4] = a7;
                    B[j * 8 + 3][i * 8 + m] = a4;
                    B[j * 8 + 3][i * 8 + m + 4] = a8;

                }
                //Transfer BT and CT to right place 
                for(m = 0; m < 4; m++){
                    a1 = B[j * 8 + m][i * 8 + 4];
                    a2 = B[j * 8 + m][i * 8 + 5];
                    a3 = B[j * 8 + m][i * 8 + 6];
                    a4 = B[j * 8 + m][i * 8 + 7];
                    a5 = A[i * 8 + 4][j * 8 + m];
                    a6 = A[i * 8 + 5][j * 8 + m];
                    a7 = A[i * 8 + 6][j * 8 + m];
                    a8 = A[i * 8 + 7][j * 8 + m];
                    
                    B[j * 8 + m][i * 8 + 4] = a5;
                    B[j * 8 + m][i * 8 + 5] = a6;
                    B[j * 8 + m][i * 8 + 6] = a7;
                    B[j * 8 + m][i * 8 + 7] = a8;
                    B[j * 8 + m + 4][i * 8] = a1;
                    B[j * 8 + m + 4][i * 8 + 1] = a2;
                    B[j * 8 + m + 4][i * 8 + 2] = a3;
                    B[j * 8 + m + 4][i * 8 + 3] = a4;
                }
                //Transpose D to DT
                for(m = 0; m < 4; m++){
                    a1 = A[i * 8 + 4 + m][j * 8 + 4];
                    a2 = A[i * 8 + 4 + m][j * 8 + 5];
                    a3 = A[i * 8 + 4 + m][j * 8 + 6];
                    a4 = A[i * 8 + 4 + m][j * 8 + 7];

                    B[j * 8 + 4][i * 8 + m + 4] = a1;
                    B[j * 8 + 5][i * 8 + m + 4] = a2;
                    B[j * 8 + 6][i * 8 + m + 4] = a3;
                    B[j * 8 + 7][i * 8 + m + 4] = a4;  
                }
            }
        }
        return;
    }
    //Matrix 61x67, use unit 16x16
    for(i = 0; i < 16; i++){
        for(j = 0; j < 16; j++){
            for(m = 0; m < 16 && i * 16 + m < N; m++){
                for(n = 0; n < 16 && j * 16 + n < M; n++){
                    B[j * 16 + n][i * 16 + m] = A[i * 16 + m][j * 16 + n];
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

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

