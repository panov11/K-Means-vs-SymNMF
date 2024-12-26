import numpy as np
import sys
np.random.seed(1234)

def error():
    """
    Prints an error message and exits the program.
    """
    print('An Error Has Occurred')
    sys.exit(1)

def get_points(file_name):
    """
    Reads points from a file and returns them as a list of lists.

    Args:
        file_name (str): The name of the input file.

    Returns:
        list: A list of points, where each point is represented as a list of floats.
    """
    points = []
    with open(file_name, 'r') as file:
        for line in file:
            clean_line = line.strip()
            if clean_line == '':  # Skip empty rows
                continue
            str_point = clean_line.split(',')
            try:
                points.append([float(p) for p in str_point])
            except:
                error()
    return points

def init_H(W, k, n):
    """
    Initializes matrix H for the symnmf algorithm.

    Args:
        W (list): The matrix W as a list of lists.
        k (int): Number of clusters.
        n (int): Number of data points.

    Returns:
        np.ndarray: The initialized matrix H.
    """
    # Convert the list of lists to a NumPy array
    array = np.array(W)

    # Flatten the array to a 1D array
    flattened_array = array.flatten()

    # Calculate m as the average of the flattened array
    m = np.mean(flattened_array)

    # Define the range for the uniform distribution
    start = 0
    end = 2 * np.sqrt(m / k)

    # Define the number of samples to generate
    num_samples = n * k

    # Generate random samples from the uniform distribution within the specified range
    samples = np.random.uniform(low=start, high=end, size=num_samples)
    H = []

    sample = 0
    for i in range(n):
        H.append([])
        for _ in range(k):
            H[i].append(samples[sample])
            sample += 1
    return np.array(H)