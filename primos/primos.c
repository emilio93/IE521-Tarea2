#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "functions.c"
// identify wether number is prime or not
int isPrime(int n) {

  // base cases
  if (n <= 1) {
    return 0;
  } else if (n <= 3) {
    return 1;
  } else if (n % 2 == 0 || n % 3 == 0) {
    return 0;
  }
  // first to check
  int i = 5;

  // check until ceil(sqrt(n))
  while (i * i <= n) {
    // check if n is divisible by multiple of i
    if (n % i == 0 || n % (i + 2) == 0) {
      return 0;
    }
    // 6 following cases already ruled out
    i = i + 6;
  }
  return 1;
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

  // setup elapsed time vars
  long start, end;
  struct timeval timecheck;
  FILE *fp;
  if (rank == 0) {

    // save start time
    gettimeofday(&timecheck, NULL);
    start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
  }

  int limit = 500000;     // min qty to find
  int chunk_size = 56000; // how many numbers per run to each rank
  int iteration = 0;      // start at iteration 0

  int local_count = 0;      // how many primes has each rank found
  int global_count = 0;     // how many ranks there are in total
  int *local_primes = NULL; // primes found in each rank

  // res req size
  int response_size = 4;
  int request_size = 4;

  /**
   * 0 : min
   * 1 : max
   * 2 : valid
   * 3 : done
   */
  int *request = NULL;

  int *response = NULL;
  int done = 0;
  int next = 0;
  // iterate rows on ranks

  if (rank != 0) {
    // assume case, eaach rank has same no of primes so give double that
    local_primes = malloc(sizeof(int) * limit / (size - 1) * 2);

    // fill with 0's
    for (size_t i = 0; i < limit / size; i++) {
      local_primes[i] = 0;
    }
  }

  do {
    // send  min(rows left, ranks) chunks
    // to first available ranks
    if (rank == 0) {
      // send a message to each rank
      for (int i = 1; i < size; i++) {
        // message is a request
        request = malloc(sizeof(int) * request_size);
        request[0] = iteration * chunk_size;       // min
        request[1] = (iteration + 1) * chunk_size; // max
        request[2] = 0;                            // assume invalid
        request[3] = done; // done:    are we done with the result?

        // in case we still got operations to do
        if (global_count < limit) {
          request[2] = 1; // set valid request
          iteration++;    // increase row for next chunk
        }

        // send the request
        MPI_Send(request, request_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        // release memory used for request
        free(request);
      }
    }

    // process rows
    if (rank != 0) {

      // allocate memory for the request made by root
      request = malloc(sizeof(int) * request_size);

      // Wait
      MPI_Recv(request, request_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      if (request[2] != 0) {
        // we expect another request
        next = 1;
        // obtain results
        for (size_t i = request[0]; i < request[1]; i++) {
          if (isPrime(i) > 0) {
            local_primes[local_count] = i;
            local_count++;
          }
        }
      } else {
        // if we are done then this is last iteration
        // we send back results
        if (request[3] == 1) {
          next = 0;
          // mpisend
        }
      }
      // free memory allocated for request no longer needed
      free(request);
    }

    // sum how many results there are
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD);

    // update results
    if (rank == 0) {

      // determine last iteration
      next = !done;
      done = global_count > limit ? 1 : 0;
    }

  } while (next != 0);

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    // get time at end and get elapsed time
    gettimeofday(&timecheck, NULL);
    end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
    printf("%ld milliseconds elapsed\n", (end - start));
  }

  ierr = MPI_Finalize();
  return 0;
}
