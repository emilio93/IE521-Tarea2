#include <mpi.h>
#include <stdio.h>

/**
 * Send message to next process, starting from 0 and ending in size-1
 * executing in a ordered maner
 */
int main(int argc, char *argv[]) {
  int rank, root_process, ierr, size;
  MPI_Status status;

  int max_value = 16;

  ierr = MPI_Init(&argc, &argv);

  /*
   * Find process ID, and how many processes were started
   */
  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &size);

  int data = 0;

  /**
   * process 0 increments counter and sends a message to process 1
   * process 1 receives message and prints data value
   * this is done max_value times
   */
  while (data < max_value) {
    if (rank == 0) {
      data++;
      MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
      MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("process %d: data is: %d\n", rank, data);
    }
  }

  ierr = MPI_Finalize();
  return 0;
}
