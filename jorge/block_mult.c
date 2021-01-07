#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>

void matmult_nat(int m, int n, int k, double **A, double **B, double **C) {
    
    int i1, i2, i3;
    for(i1 = 0; i1 < m; i1++){
		for(i2 = 0; i2 < n; i2++){
	    	C[i1][i2] = 0;
	 	}
    }
  
    for(i1 = 0; i1< m; i1++){
    	for(i2 = 0; i2 < n; i2++){
	    	for(i3 = 0; i3 < k; i3++){
				C[i1][i2]+=A[i1][i3]*B[i3][i2];
	    	}
        }
    }
}

void printMatrix(double **arr, int m, int n){
	int i, j;
	
	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			printf("%.0f \t\t", arr[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

double ** malloc_2d(int m, int n){
	// initializes a matrix of size mxn. It is needed in order to make the matrix passable as an argument to other functions. DINAMICALLY ALLOCATED.
	int i, j;
	double **A;

	A = malloc(m * sizeof(double *));
	A[0] = malloc(m * n * sizeof(double));
	for(i = 1; i < m; i++){
		A[i] = A[0] + i*n;
	}
	
	return A;
}


void initMatrix(double **arr, int m, int n){
	int i, j;

	/* setting the seed */
    srand((unsigned)time(NULL));

    for(i = 0; i < m; i++){
        for(j = 0; j < n; j++){
			// arr[i][j] = i*j;
            arr[i][j] = rand() % 20;
		}
	}
}

void pointBlock(int row, int col, int bs, double **A, double **Z){
    // This functions takes Z, which is a matrix bs*bs and makes the pointers stored at Z[0], Z[1] to point at the locations &A[row][col].
    // In this way we make Z point at the values of A we want for the block.
    // Z needs to be initialized with: Z = malloc(bs * sizeof(double *)); before being used.

    int i;
    for(i = 0; i < bs; i++){
        Z[i] = &A[row+i][col];
    }
}

void getBlock(int row, int col, int bs, double **A, double **Z){
    // This functions copies the values from A we want for the block into a matrix Z of size bs x bs i.e. make a bs x bs block starting in position A[row][col].
    // Z needs to be initialized with: Z = malloc_2d(bs, bs); before being used.

    int i, j;

    for(i = 0; i < bs; i++){
        for(j = 0; j < bs; j++){
            Z[i][j] = A[row + i][col + j];
        }
    }
}


void matmult_blk_inside(int row, int col, int bs, double **ZA, double **ZB, double **C){
    int i1, i2, i3;
    for(i1 = 0; i1< bs; i1++){
    	for(i2 = 0; i2 < bs; i2++){
	    	for(i3 = 0; i3 < bs; i3++){
				C[row + i1][col + i2] += ZA[i1][i3] * ZB[i3][i2];
	    	}
        }
    }
}

void matmult_blk(int m, int n, int k, int bs, double **A, double **B, double **C){
    int i1, i2, i3;
    double **ZA, **ZB;

    // Initialize C to be a 0 matrix.
    for(i1 = 0; i1 < m; i1++){
		for(i2 = 0; i2 < n; i2++){
	    	C[i1][i2] = 0;
	 	}
    }

    // First we calculate all entries that can be subdivided in blocks of bs x bs. 
	// We allocate in memory the two matrices were we will be storing the blocks.
    // printf("%d", bs);
	ZA = malloc_2d(bs, bs);
    ZB = malloc_2d(bs, bs);

    for(i1 = 0; i1 <= m - bs; i1 += bs){
    	for(i2 = 0; i2 <= n - bs; i2 += bs){
	    	for(i3 = 0; i3 <= k - bs; i3 += bs){
				// C[i1][i2]+=A[i1][i3]*B[i3][i2];
                getBlock(i1, i3, bs, A, ZA);
                getBlock(i3, i2, bs, B, ZB);
                matmult_blk_inside(i1, i2, bs, ZA, ZB, C);
	    	}
        }
    }
}

int main(){
	int m = 4, n = 4, k = 4, bs = 2;
	double **A, **B, **C;

	A = malloc_2d(m, k);
	initMatrix(A, m, k);

	B = malloc_2d(k,n);
	initMatrix(B, k, n);
	
	C = malloc_2d(m,n);

	printf("With our function:\n");

	matmult_nat(m, n, k, A, B, C);

	printMatrix(A, m, k);
	printMatrix(B, k, n);
	printMatrix(C, m, n);

	printf("With block function:\n");

	matmult_blk(m, n, k, bs, A, B, C);

	printMatrix(A, m, k);
	printMatrix(B, k, n);
	printMatrix(C, m, n);

	return 0;
}

