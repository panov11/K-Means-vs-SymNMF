#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

#define MAX_LINE_LENGTH 1024
#define MAX_ITER 300
#define EPSILON 1e-4
#define BETA 0.5

void print_error(){
    printf("An Error Has Occurred");
    exit(1); 
}
/*
Calculates the euclidean distance between 2 vectors
*/
double euclidean_distance(double arr1[], double arr2[], int d){
    double sum = 0;
    double diff;
    int i;
    for (i = 0; i < d; i++)
    {
        diff = arr1[i] - arr2[i];
        sum += pow(diff,2);
    }
    return sum;
}

/* Function to count rows and columns from the file */
void count_rows_cols(const char *file_name, int *rows, int *cols) {
    FILE *file = fopen(file_name, "r");
    char line[MAX_LINE_LENGTH];
    if (!file) {
        printf("Error: Cannot open file %s\n", file_name);
        exit(1);
    }
    *rows = 0;
    *cols = 0;
    /* Count the number of rows and columns */
    while (fgets(line, sizeof(line), file) != NULL) {
        if (*cols == 0) { /* Only count columns once, in the first row */
            char *token = strtok(line, " ,\t\n"); 
            while (token != NULL) {
                (*cols)++;
                token = strtok(NULL, " ,\t\n");
            }
        }
        (*rows)++;
    }
    fclose(file);
}

/* Reads points from a file and returns them as a 2D matrix. */
static double** read_matrix_from_file(const char* filename, int numPoints, int dimension) {
    FILE* file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH];
    double** points;
    int pointIndex = 0, dimIndex, i, j;
    points = (double**)malloc(numPoints * sizeof(double*));
    if (points == NULL) {
        fclose(file);
        print_error();
    }
    for (i = 0; i < numPoints; i++) {
        points[i] = (double*)malloc(dimension * sizeof(double));
        if (points[i] == NULL) {
            for (j = 0; j < i; j++) {  /* Free allocated memory in case of error */
                free(points[j]);
            }
            free(points);
            fclose(file);
            print_error();
        }
    }
    /* Read points into the 2D array */
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1) { /* Skip empty lines */
            char* token = strtok(line, ",");
            dimIndex = 0;
            while (token != NULL) {
                points[pointIndex][dimIndex++] = atof(token);
                token = strtok(NULL, ",");
            }
            pointIndex++;
        }
    }
    fclose(file);
    return points;
}


/* Function to print matrix in comma-separated format */
void print_matrix(double **matrix, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (j > 0) {
                printf(",");
            }
            printf("%.4f", matrix[i][j]);  /* Print with 4 decimal places */
        }
        printf("\n");
    }
}


/*
 * Multiplies two matrices A and B, storing the result in C.
 */
static void matmul(int A_rows, int A_cols, int B_cols, double* A, double* B, double* C) {
    int i, j, k;
    memset(C, 0, A_rows * B_cols * sizeof(double));
    for (i = 0; i < A_rows; i++) {
        for (j = 0; j < B_cols; j++) {
            for (k = 0; k < A_cols; k++) {
                C[i * B_cols + j] += A[i * A_cols + k] * B[k * B_cols + j];
            }
        }
    }
}

/*
 * Calculates the Frobenius norm squared of the difference between two matrices.
 */
static double frobeniusNormSqr(int n, int k, double* H_new, double* H_old) {
    double norm = 0.0;
    int i;
    for (i = 0; i < n * k; i++) {
        double diff = H_new[i] - H_old[i];
        norm += diff * diff;
    }
    return norm;
}

/*
 * Transposes the given matrix.
 */
static void transpose(int rows, int cols, double* matrix, double* transposed) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            transposed[j * rows + i] = matrix[i * cols + j];
        }
    }
}

/* Updates the matrix H using the symnmf algorithm. */
static double* update_H(int n, int k, double* W, double* H) {
    int iter, i, j, idx;
    double* WH = (double*)malloc(n * k * sizeof(double));
    double* H_T = (double*)malloc(k * n * sizeof(double));
    double* HHT = (double*)malloc(n * n * sizeof(double));
    double* HHTH = (double*)malloc(n * k * sizeof(double));
    double* H_new = (double*)malloc(n * k * sizeof(double));
    if (WH == NULL || H_T == NULL || HHT == NULL || HHTH == NULL || H_new == NULL) {
        print_error();
    }
    for (iter = 0; iter < MAX_ITER; iter++) {
        /* Compute WH (n x k) = W (n x n) * H (n x k) */
        matmul(n, n, k, W, H, WH);
        /* Compute H^T (k x n) */
        transpose(n, k, H, H_T);
        matmul(n, k, n, H, H_T, HHT);
        matmul(n, n, k, HHT, H, HHTH);
        /* Update H */
        for (i = 0; i < n; i++) {
            for (j = 0; j < k; j++) {
                idx = i * k + j;
                H_new[idx] = H[idx] * (1 - BETA + BETA * WH[idx] / (HHTH[idx]));
            }
        }
        /* Check convergence */
        if (frobeniusNormSqr(n, k, H_new, H) < EPSILON) {
            break;
        }
        /* Copy H_new to H */
        memcpy(H, H_new, n * k * sizeof(double));
    }
    free(WH);
    free(H_T);
    free(HHT);
    free(HHTH);
    return H_new;
}

/*
 * Calculates the symnmf of W using the given initial H.
 */
double* symnmf(int n, int k, double* W, double* H) {
    double* H_new = update_H(n, k, W, H);
    return H_new;
}

/*
Build a matrix with -1.0 in all entries, to use in the sym function
*/
double **build_matrix(int rows, int cols){
    int i, j;
    double **A = malloc(rows * sizeof(double*));
    if (A == NULL){
        print_error();
    }
    for (i = 0; i < rows; i++)
    {
        A[i] = calloc(cols,sizeof(double));
        if (A[i] == NULL){
            print_error();
        }
        for (j = 0; j < cols; j++) {
            A[i][j] = -1.0;
        }
    }
    return A;
}

double **sym (double **X, int rows, int cols){
    int i, j;
    double **A = build_matrix(rows, rows);
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < rows; j++)
        {
            if (i == j){
                A[i][j] = 0.0;
            }
            else{
                if (A[j][i] != -1.0){
                    A[i][j] = A[j][i];
                }
                else{
                    A[i][j] = exp(-0.5 * (euclidean_distance(X[i],X[j],cols)));
                }
            }
        }
    }
    return A;
}

double **ddg(double **X, int rows, int cols){
    int i,j;
    double sum;
    double **D = sym(X, rows, cols);
    for (i = 0; i < rows; i++)
    {
        sum = 0;
        for (j = 0; j < rows; j++)
        {
            sum += D[i][j];
            if (i != j){
                D[i][j] = 0;
            }   
        }
        D[i][i] = sum;
    }
    return D;
}

double **norm(double **X, int rows, int cols){
    int i, j;
    double **D_inv_sqrt;
    double **A = sym(X, rows, cols);
    double **D = ddg(X, rows, cols);
    double **W = malloc(rows * sizeof(double *));
    if (W == NULL) { print_error(); }
    for (i = 0; i < rows; i++) {
        W[i] = calloc(rows, sizeof(double));
        if (W[i] == NULL) { print_error(); }
    }
    /* Calculate D^(-1/2) */
    D_inv_sqrt = malloc(rows * sizeof(double *));
    if (D_inv_sqrt == NULL) { print_error(); }
    for (i = 0; i < rows; i++) {
        D_inv_sqrt[i] = calloc(rows, sizeof(double));
        if (D_inv_sqrt[i] == NULL) { print_error(); }
        if (D[i][i] > 0) {
            D_inv_sqrt[i][i] = 1.0 / sqrt(D[i][i]);
        }     
        else {
            D_inv_sqrt[i][i] = 0;  /* Handle zero entries */
        }
    }
    /* Calculate W = D^(-1/2) * A * D^(-1/2) */
    for (i = 0; i < rows; i++) {
        for (j = 0; j < rows; j++) {
            W[i][j] = D_inv_sqrt[i][i] * A[i][j] * D_inv_sqrt[j][j];
        }
    }
    for (i = 0; i < rows; i++) {
        free(D_inv_sqrt[i]);
        free(A[i]);
        free(D[i]);
    }
    free(D_inv_sqrt);
    free(A);
    free(D);
    return W;
}

/* Main function */
int main(int argc, char *argv[]) {
    double **data_points, **result;
    int i, rows, cols;
    const char *goal, *file_name;
    if (argc != 3) {
        printf("Usage: %s <goal> <file_name>\n", argv[0]);
        return 1;
    }
    goal = argv[1];
    file_name = argv[2];
    /* Count rows and columns from the file */
    count_rows_cols(file_name, &rows, &cols);
    /* Read matrix from file */
    data_points = read_matrix_from_file(file_name, rows, cols);
    result = NULL;
    if (strcmp(goal, "sym") == 0) {
        result = sym(data_points, rows, cols);  /* Call sym function */
    } else if (strcmp(goal, "ddg") == 0) {
        result = ddg(data_points, rows, cols);  /* Call ddg function */
    } else if (strcmp(goal, "norm") == 0) {
        result = norm(data_points, rows, cols);  /* Call norm function */
    } else {
        printf("Invalid goal. Use 'sym', 'ddg', or 'norm'.\n");
        return 1;
    }
    /* Print the result matrix */
    print_matrix(result, rows, rows);  /* Assuming the result will have dimensions (rows x rows) */
    /* Free allocated memory */
    for (i = 0; i < rows; i++) {
        free(data_points[i]);
        free(result[i]);
    }
    free(data_points);
    free(result);
    return 0;
}