#ifndef SYMNMF_H
#define SYMNMF_H

double **sym(double **X, int rows, int cols);
double **ddg(double **X, int rows, int cols);
double **norm(double **X, int rows, int cols);
double *symnmf(int n, int k, double* W, double* H);
#endif
