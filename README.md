# Clustering Comparison: `k-means` vs. `symNMF`

This repository provides a comparative analysis of the popular `k-means` clustering algorithm and the `symNMF` (Symmetric Non-Negative Matrix Factorization) method. It includes implementations in both Python and C, detailed testing, and a robust evaluation framework designed to explore their differences in accuracy, performance, and scalability.

---

## Project Overview

Clustering is a cornerstone of data analysis and machine learning, used extensively in applications such as image segmentation, customer segmentation, and document clustering. While `k-means` has been a go-to algorithm for decades due to its simplicity and efficiency, it struggles with certain data types and complex clustering scenarios. `symNMF` has gained attention as a powerful alternative, particularly effective for symmetric, non-negative data matrices.

This project evaluates the two algorithms across various datasets, examining their:
1. **Accuracy**: Quality of clustering results.
2. **Performance**: Computational efficiency and scalability.
3. **Suitability**: Situations where one method outperforms the other.

### Objectives:
- Implement `k-means` and `symNMF` algorithms for direct comparison.
- Benchmark their performance and clustering accuracy.
- Provide tools and scripts for testing and reproducibility.

---

## Repository Structure

The project consists of Python scripts, C modules, and testing utilities:

### Python Code:
- **`kmeans.py`**: Contains the implementation of the `k-means` algorithm, including initialization methods, convergence checks, and result visualization.
- **`symnmf.py`**: Python implementation of the `symNMF` algorithm, leveraging matrix factorization techniques.
- **`utils.py`**: Utility functions for data preprocessing, metric calculations (e.g., silhouette score, purity), and visualization.
- **`analysis.py`**: High-level script for running comparative experiments and generating statistical summaries of results.

### C Code:
- **`symnmf.c` and `symnmfmodule.c`**: High-performance `symNMF` implementation written in C, designed for large-scale data.
- **`symnmf.h`**: Header file for the `symNMF` C module.
- **`Makefile`**: Script for building and compiling the C modules for Python integration.

### Testing Utilities:
- **`compareTester.sh`**: A comprehensive testing script that:
  - Validates correctness of Python and C implementations.
  - Benchmarks execution time and memory usage.
  - Ensures robustness through Valgrind-based memory leak checks.

### Setup:
- **`setup.py`**: Configuration file for Python-C extension building and installation.

---

## How to Run

### Prerequisites:
- Python 3.8+ (tested compatibility)
- GCC (for compiling the C code)
- Valgrind (optional, for memory management checks)

### Installation:
1. Clone the repository:
   ```bash
   git clone <repository_url>
   cd <repository_folder>
   ```
2. Install Python dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Compile the C code:
   ```bash
   make
   ```

### Running the Analysis:
Run the main analysis script to evaluate both algorithms:
```bash
python analysis.py
```

To test the implementations and benchmarks:
```bash
bash compareTester.sh
```

---

## Results and Insights

The comparison between `k-means` and `symNMF` yielded the following insights:

### Accuracy:
- `symNMF` consistently achieved **1%–1.5% higher accuracy** compared to `k-means` across various datasets.
- It proved particularly effective for data with symmetrical, non-negative structures, where matrix factorization approaches excel.

### Performance:
- **Execution Time**: `k-means` was faster on small datasets due to its lightweight iterative approach.
- **Scalability**: `symNMF` demonstrated superior scalability and stability on larger datasets, particularly for high-dimensional data.

### Use Cases:
- `k-means`: Ideal for quick and straightforward clustering tasks.
- `symNMF`: Best suited for advanced scenarios requiring higher accuracy and robustness to complex data structures.

---

## Testing and Validation

The `compareTester.sh` script provides an automated pipeline for:
1. Validating the functional correctness of all algorithms.
2. Measuring execution time for both Python and C implementations.
3. Ensuring memory safety through Valgrind tests.

To run the tests:
```bash
bash compareTester.sh
```

---

## Future Work

Potential directions for enhancing this project include:
1. Extending the comparison to additional clustering algorithms such as DBSCAN and hierarchical clustering.
2. Optimizing the C implementation of `symNMF` for even greater efficiency.
3. Adding support for GPU-accelerated computations using libraries like CUDA.

---

## Authors and Acknowledgments

This project was developed as part of a study on clustering algorithms. Special thanks to all contributors for their insights and collaboration.

For questions or contributions, please contact yonathan.atid@gmail.com.

---
