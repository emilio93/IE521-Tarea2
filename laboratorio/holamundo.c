#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Obtains part of average of a chunk of an array
 *
 * The sum of all the part of averages obtains the average of an array.
 *
 * @param chunk part of the array
 * @param chunk_size size of the chunk
 * @param data_size size of the full array
 * @param initial first element used for averaging, normally 0.
 * @return float part of the average corresponding to the chunk
 */
float average(int chunk[], int chunk_size, int data_size, int initial) {

  // initialize result var
  float result = 0;

  // obtain average based on full list size
  // s: full list
  // n: qty of items in full list
  // <s>: full list average
  //
  // <s> = (s[0] + s[1] + s[2] + ... + s[n-1])/n
  //     = s[0]/n + s[1]/n + s[2]/n + ... + s[n-1]/n
  //     = (s[0] + s[1])/n + (s[2] + s[4])/n + ... + (s[n-2] + s[n-1])/n
  //     = ...
  //
  for (int i = initial; i < (initial+chunk_size); i++) {
    result = result + (float)chunk[i] / (float)data_size;
  }

  return result;
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

  // Generated rand nums in google spreadsheets
  // average is 514.500
  int data[] = {355, 70,  34,  305, 571, 542, 133, 704, 648, 649, 775, 759,
                383, 638, 724, 543, 702, 389, 657, 675, 369, 92,  738, 893};
  int data_size = sizeof(data) / sizeof(data[0]);

  if (rank == 0) {
    // average calculation of the whole list chunk
    float result = average(data, data_size, data_size, 0);
    // print single process result
    printf("Single process 0 average is %f\n", result);
  }

  // scatter the array chunks of size floor(data_size / size)(division truncates
  // value to fit int) to each process
  int *chunk = malloc(sizeof(int) * data_size / size);
  MPI_Scatter(data, data_size / size, MPI_INT, chunk, data_size / size, MPI_INT,
              0, MPI_COMM_WORLD);

  // calculate part of average from each chunk
  float chunk_average = average(chunk, data_size / size, data_size, 0);

  // create float pointer to pass the averages from each process
  float *chunk_averages = NULL;

  // asign memory to root process only as this is the
  // process that gathers and sums the results.
  if (rank == 0) {
    chunk_averages = (float *)malloc(sizeof(float) * size);
  }

  // gather the results
  MPI_Gather(&chunk_average, 1, MPI_INT, chunk_averages, 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  // process the results in root process
  if (rank == 0) {

    // initial result is 0
    float result = 0;
    // add each result
    for (size_t i = 0; i < size; i++) {
      result = result + chunk_averages[i];
    }

    // complete the result with last chunk(if exists), which is smaller than
    // other chunks, specifially data_size % size, note this chunk size will
    // be 0 if the number of elements in the full list is divisible by the
    // qty of processes available
    int remaining_elements = data_size % size;
    if (remaining_elements != 0) {
      int first_element = data_size - remaining_elements;
      result = result + average(data, remaining_elements, data_size, first_element);
    }

      // print multi process result
    printf("Multi process average is %f\n", result);
    free(chunk_averages);
  }

  free(chunk);

  ierr = MPI_Finalize();
  return 0;
}
