#include <mpi.h>
#include <stdio.h>

/*
 * Create child processes,each of which has its own variables.
 * From this point on, every process executes a separate copy
 * of this program. Each process has adifferent processID,
 * ranging from 0 to numprocs minus 1, and COPIES of all
 * variables defined in the program. No variables are shared.
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

  if (rank % 2 == 0) {
    printf("Hello! My ID is an even number. I am %d of %d\n", rank, size);
  } else {
    printf("Hello! My ID is an odd number. I am %d of %d\n", rank, size);
  }

  ierr = MPI_Finalize();
  return 0;
}
