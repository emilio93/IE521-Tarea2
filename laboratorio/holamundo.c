#include <mpi.h>
#include <stdio.h>

/**
 * Send message to next process, starting from 0 and ending in size-1
 * executing in a ordered maner
 */
int main(int argc, char *argv[]) {
  int rank, root_process, ierr, size;
  MPI_Status status;

  ierr = MPI_Init(&argc, &argv);

  /*
   * Find process ID, and how many processes were started
   */
  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &size);

  int data;

  // wait for message before sending
  MPI_Recv(&data, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  if (rank % 2 == 0) {
    printf("Hello! My ID is an even number. I am %d of %d\n", rank, size);
  } else {
    printf("Hello! My ID is an odd number. I am %d of %d\n", rank, size);
  }

  // Send to next, last will send to first so everyone has something to listen
  MPI_Send(&data, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);

  ierr = MPI_Finalize();
  return 0;
}
