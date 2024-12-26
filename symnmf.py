import sys
import os
import symnmf as sy
import numpy as np
import utils

# Seed the random number generator for reproducibility
np.random.seed(1234)

def error():
    """
    Prints an error message and exits the program.
    """
    print('An Error Has Occurred')
    sys.exit(1)

def convert_args():
    """
    Converts and validates command-line arguments.
    Returns:
        tuple: (k, goal, file_name)
        k (int): The number of clusters for symnmf.
        goal (str): The goal of the operation (symnmf, sym, ddg, norm).
        file_name (str): The name of the input file.
    """
    def k_to_int(k):
        try:
            return int(k)
        except:
            error()

    def validate_goal(goal):
        if goal not in ["symnmf", "sym", "ddg", "norm"]:
            error()

    def validate_file_exist(file_name):
        if not os.path.exists(file_name):
            error()

    if len(sys.argv) < 4:
        error()
    # Read arguments
    k = sys.argv[1]
    goal = sys.argv[2]
    file_name = sys.argv[3]
    k = k_to_int(k)
    validate_goal(goal)
    validate_file_exist(file_name)
    return (k, goal, file_name)

def do_sym(points):
    """
    Executes the sym operation.

    Args:
        points (list): The data points.
    """
    return sy.sym(points)

def print_matrix(matrix,n_cols):
    """
    Prints the matrix with values formatted to 4 decimal places, used by the symnmf function.

    Args:
        matrix (list): The matrix to be printed (1D matrix).
        n_cols (int): number of colunms
    """
    for i in range(0, len(matrix), n_cols):
        print(",".join(f"{val:.4f}" for val in matrix[i:i + n_cols]))

def print_matrix2(matrix):
    """
    Prints the matrix with values formatted to 4 decimal places, used by the ddg, norm and sym functions.

    Args:
        matrix (list): The matrix to be printed (2D matrix).
    """
    for row in matrix:
        print(",".join(f"{val:.4f}" for val in row))


def do_symnmf(n,points, k):
    """
    Executes the symnmf operation.

    Args:
        n (int): Number of data points.
        points (list): The data points.
        k (int): Number of clusters for symnmf.
    """
    # Calculate W
    W_flat = sy.norm(points)
    W = np.reshape(W_flat, (n, n))
    W.flatten()
    # Initialize H matrix
    H = utils.init_H(W, k, n).tolist()
    W = W.tolist()
    return sy.calcSymnmf(n,k,W,H)


def do_ddg(points):
    """
    Executes the ddg operation.

    Args:
        points (list): The data points.
    """
    return sy.ddg(points)

def do_norm(points):
    """
    Executes the norm operation.

    Args:
        points (list): The data points.
    """
    return sy.norm(points)

def main():
    """
    Main function to execute the appropriate operation based on command-line arguments.
    """
    k, goal, file_name = convert_args()
    points = utils.get_points(file_name)
    n = len(points)
    d = len(points[0])
    if goal == "symnmf":
        h = do_symnmf(n,points, k)
        np.reshape(h,(n,k)).tolist()
        print_matrix(h,k)
    elif goal == "sym":
        A = do_sym(points)
        print_matrix2(A)
    elif goal == "ddg":
        D = do_ddg(points)
        print_matrix2(D)
    elif goal == "norm":
        W = do_norm(points)
        print_matrix2(W)
    else:
        error()

if __name__ == "__main__":
    main()
