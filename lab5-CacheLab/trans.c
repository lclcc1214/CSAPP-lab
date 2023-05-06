/* 
 * trans.c - 矩阵转置B=A^T
 *每个转置函数都必须具有以下形式的原型：
 *void trans（int M，int N，int a[N][M]，int B[M][N]）；
 *通过计算，块大小为32字节的1KB直接映射缓存上的未命中数来计算转置函数。
 */ 
#include <stdio.h>
#include "cachelab.h"
#define size 17
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] = "Transpose submission";  //请不要修改“Transpose_submission”

void trans_32(int M, int N, int A[N][M], int B[M][N]);
void trans_64(int M, int N, int A[N][M], int B[M][N]);
void trans_61(int M, int N, int A[N][M], int B[M][N]);

void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{

//                          请在此处添加代码
//*************************************Begin********************************************************
    switch(M){
        case 32:
            trans_32(M, N, A, B);
            break;
        case 64:
            trans_64(M, N, A, B);
            break;
        case 61:
            trans_61(M, N, A, B);
            break;
    }
//**************************************End**********************************************************
}

void trans_32(int M, int N, int A[N][M], int B[M][N]){
    // 一个cache块大小为32字节，即8个int，cache存下8行，所以分割成8*8的矩阵块
    int i, j, k;
    int temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
    for (i = 0; i < N; i += 8){ //遍历每8行
        for (j = 0; j < M; j += 8){ // 遍历每8列
            for (k = 0; k < 8; k++, i++){ //遍历矩阵块
                // 只会miss一次
                temp1 = A[i][j];
                temp2 = A[i][j + 1];
                temp3 = A[i][j + 2];
                temp4 = A[i][j + 3];
                temp5 = A[i][j + 4];
                temp6 = A[i][j + 5];
                temp7 = A[i][j + 6];
                temp8 = A[i][j + 7];
                //第一次全miss，后面除了对角线上的都会hit
                B[j][i] = temp1;
                B[j + 1][i] = temp2;
                B[j + 2][i] = temp3;
                B[j + 3][i] = temp4;
                B[j + 4][i] = temp5;
                B[j + 5][i] = temp6;
                B[j + 6][i] = temp7;
                B[j + 7][i] = temp8;
            }
            i -= 8;
        }
    }
}

void trans_64(int M, int N, int A[N][M], int B[M][N]){
    // 考虑 8 × 8 分块，由于存在着每 4 行就会占满一个缓存的问题，
    // 把分块内部分成 4 个 4 × 4 的小分块分别处理
    int i, j, k;
    int temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
    for (i = 0; i < N; i += 8){ //遍历每8行
        for (j = 0; j < M; j += 8){ // 遍历每8列
            // step1: 将A11和A12翻转到B11和B12中
            for (k = 0; k < 4; k++, i++){ 
                // 只会miss一次 取A11和A12
                temp1 = A[i][j];
                temp2 = A[i][j + 1];
                temp3 = A[i][j + 2];
                temp4 = A[i][j + 3];
                temp5 = A[i][j + 4];
                temp6 = A[i][j + 5];
                temp7 = A[i][j + 6];
                temp8 = A[i][j + 7];
                //全miss，后面除了对角线上的都会hit
                B[j][i] = temp1;    // 左上 放B11
                B[j + 1][i] = temp2;
                B[j + 2][i] = temp3;
                B[j + 3][i] = temp4;
                //全hit
                B[j][i + 4] = temp5;    // 右上 放B12
                B[j + 1][i + 4] = temp6;
                B[j + 2][i + 4] = temp7;
                B[j + 3][i + 4] = temp8;
            }
            i -= 4;
            // step2: 先将A21翻转到B12，将B12翻转到B21
            for (k = 0; k < 4; k++, j++){
                // 全miss, 后面都会hit 取A21
                temp1 = A[i + 4][j];
                temp2 = A[i + 5][j];
                temp3 = A[i + 6][j];
                temp4 = A[i + 7][j];
                // 只会miss一次 取B12
                temp5 = B[j][i + 4];
                temp6 = B[j][i + 5];
                temp7 = B[j][i + 6];
                temp8 = B[j][i + 7];
                // 全hit A21放B12
                B[j][i + 4] = temp1;
                B[j][i + 5] = temp2;
                B[j][i + 6] = temp3;
                B[j][i + 7] = temp4;
                // 只会miss一次 B12放B21
                B[j + 4][i] = temp5;
                B[j + 4][i + 1] = temp6;
                B[j + 4][i + 2] = temp7;
                B[j + 4][i + 3] = temp8;
            }
            j -= 4;
            //step3: 将A22翻转到B22
            j += 4;
            i += 4;
            for (k = 0; k < 4; k++, i++){
                // 只会miss一次
                temp1 = A[i][j];
                temp2 = A[i][j + 1];
                temp3 = A[i][j + 2];
                // 错误原因：temp3 = A[i][j + 3];
                temp4 = A[i][j + 3];
                //全miss，后面都会hit
                B[j][i] = temp1;
                B[j + 1][i] = temp2;
                B[j + 2][i] = temp3;
                B[j + 3][i] = temp4;
            }
            i -= 8;
            j -= 4;
        }
    }
}



void trans_61(int M, int N, int A[N][M], int B[M][N]){
    // 分块 size*size
    int i, j;
    for (i = 0; i < N; i += size){
        for (j = 0; j < M; j += size){
            for (int x = i; x < N && x < i + size; x++){
                for (int y = j; y < M && y < j + size; y++)
                    B[y][x] = A[x][y];
            }
        }
    }
}

/* 
 * 我们在下面定义了一个简单的方法来帮助您开始，您可以根据下面的例子把上面值置补充完整。
 */ 

/* 
 * 简单的基线转置功能，未针对缓存进行优化。
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
 * registerFunctions-此函数向驱动程序注册转置函数。
 *在运行时，驱动程序将评估每个注册的函数并总结它们的性能。这是一种试验不同转置策略的简便方法。
 */
void registerFunctions()
{
    /* 注册解决方案函数  */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* 注册任何附加转置函数 */
    //registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - 函数检查B是否是A的转置。在从转置函数返回之前，可以通过调用它来检查转置的正确性。
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

