#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int rank, root_process, ierr, size;
  MPI_Status status;

  ierr = MPI_Init(&argc, &argv);

  /*
   * Find process ID, and how many processes were started
   */
  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &size);

  int data = 295;

  /**
   * process 0 is the first to send to next process and waits for a message
   * every other process also waits
   * following processes send to next, last process sends to process 0 who's
   * waiting for a message from last process
   */
  if (rank == 0) {
    MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    printf("process %d sent data %d to process %d\n", rank, data,
           (rank + 1) % size);
    MPI_Recv(&data, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("process %d: data is: %d\n", rank, data);
  } else {
    MPI_Recv(&data, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("process %d received data %d from process %d\n", rank, data,
           rank - 1);
    MPI_Send(&data, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
    printf("process %d sent data %d to process %d\n", rank, data,
           (rank + 1) % size);
  }

  ierr = MPI_Finalize();
  return 0;
}
