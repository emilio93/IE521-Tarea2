#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
 * @param a vector data
 * @param b transposed matrix data
 * @param n width of b matrix
 * @param m length of vector, height of b matrix
 * @param j row where vector is located at within a matrix
 * @return int dot product result
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

int main(int argc, char *argv[]) {
  int rank, root_process, ierr, size;
  MPI_Status status;

  ierr = MPI_Init(&argc, &argv);

  /*
   * Find process ID, and how many processes were started
   */
  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &size);

  // create vector
  int vector_size = 8;
  int vector[] = {1, 1, 1, 1, 1, 1, 1, 1};

  // create matrix
  int matrix_column_count = 8;
  int matrix_row_count = 8;
  int matrix_size = matrix_column_count * matrix_row_count;
  // clang-format off
  int matrix[] = {
    1,0,0,0,0,0,0,0,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,1,
    1,0,0,0,0,1,1,1,
    1,0,0,0,1,1,1,1,
    1,0,0,1,1,1,1,1,
    1,0,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1
  };
  // clang-format on

  // transpose matrix
  int *transposed =
      malloc(sizeof(int) * matrix_row_count * matrix_column_count);
  transpose(matrix, transposed, matrix_row_count, matrix_column_count);

  if (rank == 0) {
    printf("Vector is:\n");
    print_matrix(vector, vector_size, 1);
    printf("Matrix is:\n");
    print_matrix(matrix, matrix_row_count, matrix_column_count);
  }

  // process in each rank to obtain a single result item.
  int result = vector_product_transposed_vector(
      vector, transposed, matrix_row_count, matrix_column_count, rank);

  // each rank but 0 sends result to 0
  if (rank != 0) {
    MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  // obtain
  if (rank == 0) {
    int *data = malloc(sizeof(int) * vector_size);

    // obtained result by rank 0 is easily available
    data[0] = result;

    // obtain results from other ranks
    for (size_t i = 1; i < size; i++) {
      MPI_Recv(&data[i], 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
    }

    // print result
    printf("Result is:\n");
    print_matrix(data, matrix_row_count, 1);
  }

  ierr = MPI_Finalize();
  return 0;
}
