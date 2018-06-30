#ifndef DEF_MAIN
#define DEF_MAIN

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "functions.c"

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
  // int vector[] = {1, 1, 2, 3, 4, 5, 6, 7};
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
  // int matrix[] = {
  //   1,0,0,0,0,0,0,0,
  //   1,0,0,0,0,0,0,1,
  //   1,0,0,0,0,0,3,1,
  //   1,0,0,0,0,1,3,1,
  //   1,0,0,0,2,1,1,1,
  //   1,0,0,1,2,1,1,1,
  //   1,0,2,1,2,1,1,1,
  //   1,3,2,1,2,1,1,1
  // };
  // clang-format on

  // print data for the product
  if (rank == 0) {
    printf("vector is:\n");
    print_matrix(vector, vector_size, 1);
    printf("matrix is:\n");
    print_matrix(matrix, matrix_column_count, matrix_row_count);
  }

  // check correct size for product operation
  if (vector_size != matrix_row_count) {
    printf("vector_size(%d) must equal matrix_row_size(%d)\n", vector_size,
           matrix_row_count);
    return 0;
  }

  // get transposed of matrix
  int *transposed =
      malloc(sizeof(int) * matrix_column_count * matrix_row_count);
  transpose(matrix, transposed, matrix_column_count, matrix_row_count);

  int next;            // indicates whether the loop should do another iteration
  int row = 0;         // indicates row from result to assign
  int *request = NULL; // request to be made by root
  int *response = NULL; // response to be made by other ranks
  int done = 0;         // indicates the result is complete.

  int *results = NULL; // results from product
  if (rank == 0) {
    results = malloc(sizeof(int) * vector_size); // results from product
  }

  // iterate rows on ranks
  do {

    // send  min(rows left, ranks) rows
    // to first available ranks
    if (rank == 0) {
      // send a message to each rank
      for (int i = 1; i < size; i++) {
        // message is a request, request is 3 ints
        request = malloc(sizeof(int) * 3);
        request[0] = 0;    // isValid: assume invalid request
        request[1] = row;  // row:     requested row result
        request[2] = done; // done:    are we done with the result?

        // in case we still got operations to do
        if (row < vector_size) {
          request[0] = 1; // set valid request
          row++;          // increase row for next rank
        }

        // send the request
        MPI_Send(request, 3, MPI_INT, i, 0, MPI_COMM_WORLD);

        // release memory used for request
        free(request);
      }
    }

    // process rows
    if (rank != 0) {

      // allocate memory for the request made by root
      request = malloc(sizeof(int) * 3);

      // Wait
      MPI_Recv(request, 3, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      // allocate memory for the response to root
      response = malloc(sizeof(int) * 3);

      // check if we got valid request
      // if request[0] != 0, then it is a valid request
      if (request[0] != 0) {
        // we expect another request
        next = 1;
        // prepare response data
        response[0] = 1; // valid
        // obtain result for single row
        response[1] = vector_product_transposed_vector(
            vector, transposed, matrix_column_count, matrix_row_count,
            request[1]);
        // assigned_row for further ordering by root
        response[2] = request[1];
      } else {
        // forward invalid request to the response
        response[0] = 0; // invalid

        // if we are done then this is last iteration
        if (request[2] == 1) {
          next = 0;
        }
      }
      // free memory allocated for request no longer needed
      free(request);

      // send the response back to root
      MPI_Send(response, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);

      // free memory allocated for response no longer needed
      free(response);
    }

    // update results
    if (rank == 0) {

      // assume this is las iteration for rank 0
      next = 0;

      // on each rank
      for (size_t i = 1; i < size; i++) {
        // receive response
        request = malloc(sizeof(int) * 3);
        MPI_Recv(request, 3, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        // if the response is valid
        if (request[0] != 0) {
          // put result(request[1]) in corresponding row(request[2])
          results[request[2]] = request[1];
          // assume we still got another set of requests to make
          next = 1;
        }
        // free memory allocated for request no longer needed
        free(request);
      }

      // done is normally set to 0, and is only set to 1
      // whenever next is 0, this is, when all requests/responses were
      // invalid
      // when done is 1, then next is 0 in rank 0, which takes it out
      // of the loop, all other ranks have already been taken out
      if (done == 1) {
        next = 0;
      } else if (next == 0) {
        done = 1;
        next = 1;
      }
    }
  } while (next != 0);

  // print result
  if (rank == 0) {
    printf("result is:\n");
    print_matrix(results, vector_size, 1);
  }
  // free memory used for transposed matrix
  free(transposed);

  ierr = MPI_Finalize();
  return 0;
}

#endif
