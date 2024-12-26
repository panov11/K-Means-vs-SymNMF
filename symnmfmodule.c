#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"

/* Converts a Python list to C array*/
int convert_Pylist_to_carray(PyObject* py_list, double* c_array, int rows, int cols) {
    if (!PyList_Check(py_list)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list of lists");
        printf("An Error Has Occurred");
        exit(1); 
    }
    for (int i = 0; i < rows; i++) {
        PyObject* row = PyList_GetItem(py_list, i);
        if (!PyList_Check(row)) {
            PyErr_SetString(PyExc_TypeError, "Expected a list of lists");
            printf("An Error Has Occurred");
            exit(1); 
        }

        for (int j = 0; j < cols; j++) {
            PyObject* item = PyList_GetItem(row, j);
            c_array[i * cols + j] = PyFloat_AsDouble(item);
        }
    }
    return 0;
}

/*Convert C array of arrays to a Python list of lists*/
static PyObject* convert_to_list_of_lists(double** A, int rows, int cols){
    PyObject *result = PyList_New(rows);
    for (int i = 0; i < rows; i++) {
        PyObject *row = PyList_New(rows);
        for (int j = 0; j < cols; j++) {
            PyList_SetItem(row, j, PyFloat_FromDouble(A[i][j]));
        }
        PyList_SetItem(result, i, row);
    }
    return result;
}

/* Converts a C array to a Python list */
static PyObject* cArrayToPyList(double* array, int n) {
    PyObject* list = PyList_New(n);
    int i;
    if (!list) {
        printf("An Error Has Occurred");
        exit(1); 
    }
    for (i = 0; i < n; ++i) {
        PyObject* value = PyFloat_FromDouble(array[i]);
        if (!value) {
            Py_DECREF(list);
            printf("An Error Has Occurred");
            exit(1); 
        }
        PyList_SET_ITEM(list, i, value);
    }
    return list;
}

/* Updates the number of rows and cols of a Python list of lists*/
void get_rows_and_columns(PyObject *list_of_lists, int *rows, int *cols) {
    // Check if the input is a list
    if (!PyList_Check(list_of_lists)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list of lists");
        printf("An Error Has Occurred");
        exit(1); 
    }
    // Get the number of rows (outer list size)
    *rows = PyList_Size(list_of_lists);
    // Initialize cols to 0
    *cols = 0;
    if (*rows > 0) {
        // Get the first element (assuming it's a list)
        PyObject *first_row = PyList_GetItem(list_of_lists, 0);

        if (PyList_Check(first_row)) {
            // Get the number of columns (size of the first inner list)
            *cols = PyList_Size(first_row);
        } else {
            PyErr_SetString(PyExc_TypeError, "Expected a list of lists");
        }
    }
}

double **build_matrix2(PyObject *data_points, int *rows, int *cols){
    int i, j;
    double **X = malloc(*rows * sizeof(double*));
    if( X == NULL){
        printf("An Error Has Occurred");
        exit(1); 
    }
    for (i = 0; i < *rows; i++)
    {
        X[i] = calloc(*cols, sizeof(double));
        if( X[i] == NULL){
            printf("An Error Has Occurred");
            exit(1); 
        }
    }
    for (i = 0; i < *rows; i++)
    {
        for (j = 0; j < *cols; j++)
        {
            PyObject *row = PyList_GetItem(data_points, i);
            PyObject *value = PyList_GetItem(row, j);
            X[i][j] = PyFloat_AsDouble(value);
        } 
    }
    return X;
}

/* Function to free allocated memory for matrices*/
void free_matrix(double **X, int *rows){
    int i;
    for (i = 0; i < *rows; i++)
    {
        free(X[i]);
    }
    free(X);
}

/* Sym function */
static PyObject* sym_fit(PyObject *self, PyObject *args) {
    int rows = 0, cols = 0;
    PyObject *data_points;
    if (!PyArg_ParseTuple(args, "O", &data_points)) {
        return NULL;
    }
    get_rows_and_columns(data_points, &rows, &cols);
    if (PyErr_Occurred()) {
    return NULL;  // Return NULL if an error occurred
    }
    double **X = build_matrix2(data_points, &rows, &cols);
    double **A = sym(X, rows, cols);
    if (A == NULL) {
        free_matrix(X, &rows);
        return PyErr_NoMemory();  // Memory allocation failed in sym
    }
    PyObject *result = convert_to_list_of_lists(A, rows, rows);
    // Free the matrices X and A
    free_matrix(X, &rows);
    free_matrix(A, &rows);
    // Return the result
    return result;
}

/* Ddg function */
static PyObject* ddg_fit(PyObject *self, PyObject *args) {
    int rows = 0, cols = 0;
    PyObject *data_points;
    if (!PyArg_ParseTuple(args, "O", &data_points)) {
        return NULL;
    }
    get_rows_and_columns(data_points, &rows, &cols);
    if (PyErr_Occurred()) {
    return NULL;  // Return NULL if an error occurred
    }
    double **X = build_matrix2(data_points, &rows, &cols);
    double **A = ddg(X, rows, cols);
    if (A == NULL) {
        free_matrix(X, &rows);
        return PyErr_NoMemory();  // Memory allocation failed in sym
    }
    PyObject *result = convert_to_list_of_lists(A, rows, rows);
    // Free the matrices X and A
    free_matrix(X, &rows);
    free_matrix(A, &rows);

    // Return the result
    return result;
}

/* Norm function */
static PyObject* norm_fit(PyObject *self, PyObject *args) {
    int rows =0, cols = 0;
    PyObject *data_points;
    if (!PyArg_ParseTuple(args, "O", &data_points)) {
        return NULL;
    }
    get_rows_and_columns(data_points, &rows, &cols);
    if (PyErr_Occurred()) {
    return NULL;  // Return NULL if an error occurred
    }
    double **X = build_matrix2(data_points, &rows, &cols);
    double **A = norm(X, rows, cols);
    if (A == NULL) {
        free_matrix(X, &rows);
        return PyErr_NoMemory();  // Memory allocation failed in sym
    }
    // Convert the result matrix A back to a Python list of lists
    PyObject *result = convert_to_list_of_lists(A, rows, rows);
    // Free the matrices X and A
    free_matrix(X, &rows);
    free_matrix(A, &rows);
    // Return the result
    return result;
}

/* Python wrapper for the calcSymnmf function */
static PyObject* calcSymnmfPy(PyObject* self, PyObject* args) {
    int n, k;
    PyObject *W_list, *H_list;
    /* Parse the input tuple */
    if (!PyArg_ParseTuple(args, "iiOO", &n, &k, &W_list, &H_list)) {
        return NULL;
    }
    /* Ensure W_list and H_list are lists */
    if (!PyList_Check(W_list) || !PyList_Check(H_list)) {
        PyErr_SetString(PyExc_TypeError, "Expected list objects for W and H");
        return NULL;
    }
    /* Allocate memory for C arrays */
    double* W = (double*) malloc(n * n * sizeof(double));
    double* H = (double*) malloc(n * k * sizeof(double));
    if (W == NULL || H == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Memory allocation failed");
        printf("An Error Has Occurred");
        exit(1);
    }
    /* Convert Python lists to C arrays */
    if (convert_Pylist_to_carray(W_list, W, n, n) == -1 || convert_Pylist_to_carray(H_list, H, n, k) == -1) {
        free(W);
        free(H);
        return NULL;
    }
    double* result = symnmf(n, k, W, H);
    PyObject* pyResult = cArrayToPyList(result, n * k);
    free(result);
    /* Free allocated memory */
    free(W);
    free(H);
    return pyResult;
}

static PyMethodDef SymNMF_FunctionsTable[] = {
    {
        "calcSymnmf", // name exposed to Python
        calcSymnmfPy, // C wrapper function
        METH_VARARGS, // received variable args (but really just 1)
        "returns the final H matrix" // documentation
    },{
        "sym", // name exposed to Python
        sym_fit, // C wrapper function
        METH_VARARGS, // received variable args (but really just 1)
        "Form the similarity matrix A from X" // documentation
    }, {
        "ddg", // name exposed to Python
        ddg_fit, // C wrapper function
        METH_VARARGS, // received variable args (but really just 1)
        "Compute the diagonal degree matrix" // documentation
    }, {
        "norm", // name exposed to Python
        norm_fit, // C wrapper function
        METH_VARARGS, // received variable args (but really just 1)
        "Compute the normalized similarity W" // documentation
    }, {
        NULL, NULL, 0, NULL
    }
};

// modules definition
static struct PyModuleDef SymNMF_Module = {
    PyModuleDef_HEAD_INIT,
    "symnmf",     // name of module exposed to Python
    "SymNMF Python wrapper for custom C extension library.", // module documentation
    -1,
    SymNMF_FunctionsTable
};

PyMODINIT_FUNC PyInit_symnmf(void) {
    return PyModule_Create(&SymNMF_Module);
}