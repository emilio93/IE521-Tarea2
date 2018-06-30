#ifndef DEF_FUNCTIONS
#define DEF_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief transpose an nxm matrix into a mxn matrix
 *
 * @param input matrix start
 * @param output output matrix
 * @param n matrix column count
 * @param m matrix row count
 */
void transpose(const int *input, int *output, const int n, const int m) {
  for (size_t i = 0; i < n * m; i++) {
    output[i] = input[m * (i % n) + i / n];
  }
}

/**
 * @brief obtains dot product of vectors, a must be a vector
 * b may be an array of width m, and j-th row is used for
 * the operation.
 *
 *
 * a = 1 1 1 1 1
 * b = 1 1 1 1 1
 *     1 1 1 1 1
 *     2 2 2 2 2 <- j = 2
 *     1 1 1 1 1
 * int a[] = {1,1,1,1,1};
 * int b[] = {1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,1,1,1,1,1};
 * vector_product_transposed_vector(a, b, 5, 4, 2); // = 10
 *
 * @param a vector data
 * @param b transposed matrix data
 * @param n width of b matrix
 * @param m length of vector, height of b matrix
 * @param j row where vector is located at within a matrix
 * @return int dot product result
 *
 * example of use
 *
 * int a[] = {1,1,1,1};
 *
 */
int vector_product_transposed_vector(const int *a, const int *b, int n, int m,
                                     int j) {
  int result = 0;
  for (size_t i = 0; i < n; i++) {
    result = result + a[i] * b[m * j + i];
  }
  return result;
}

/**
 * @breif Given a m sized vector and a nxp matrix, the product vector*matrix is
 * obtained
 *
 * @param vector vector to multiply
 * @param matrix matrix to multiply
 * @param result result of product vextor x matrix
 * @param n width of matrix
 * @param m height of matrix, is equal to vector length
 */
void vector_product_matrix(const int *vector, const int *matrix, int *result,
                           int n, int m) {
  // result[j] = vector[j]*matrix[0,j] + vector[j]*matrix[1,j] + ...

  // transposing matrix takes advantge of spatial locality
  // in matrix for multiplication
  // further optimization could take advantage of
  int *transposed = malloc(sizeof(int) * n * m);
  transpose(matrix, transposed, n, m);
  for (size_t j = 0; j < n; j++) {
    result[j] = vector_product_transposed_vector(vector, transposed, n, m, j);
  }
  free(transposed);
}

/**
 * @brief prints a nxm matrix
 *
 * @param matrix the matrix to print
 * @param n width of matrix
 * @param m height of matrix
 */
void print_matrix(int *matrix, int n, int m) {
  for (size_t i = 0; i < m; i++) { // rows
    printf(i == 0 ? "(" : " ");
    for (size_t j = 0; j < n; j++) { // columns
      printf(j != n - 1 ? "%d, " : "%d", matrix[m * i + j]);
    }
    printf(i == m - 1 ? ")\n" : "\n");
  }
}

#endif // !DEF_FUNCTIONS
