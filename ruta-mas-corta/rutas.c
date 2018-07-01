#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "functions.c"

/**
 * @brief Get the paths array of all possible routes
 *
 * @param cities stops
 * @param cities_size qty of stops
 * @param paths output paths in single array
 */
void get_paths(int *cities, int cities_size, int *paths) {
  int path_count = 0;
  int path_length = 9;
  for (size_t a = 1; a < cities_size; a++) { // first destiny

    for (size_t b = 1; b < cities_size; b++) { // second destiny
      if (b == a) {
        continue;
      }
      for (size_t c = 1; c < cities_size; c++) { // third destiny
        if (c == a || c == b) {
          continue;
        }
        for (size_t d = 1; d < cities_size; d++) { // fourth destiny
          if (d == a || d == b || d == c) {
            continue;
          }
          for (size_t e = 1; e < cities_size; e++) { // fifth destiny
            if (e == a || e == b || e == c || e == d) {
              continue;
            }
            for (size_t f = 1; f < cities_size; f++) { // sixth destiny
              if (f == a || f == b || f == c || f == d || f == e) {
                continue;
              }
              for (size_t g = 1; g < cities_size; g++) { // seventh destiny
                if (g == a || g == b || g == c || g == d || g == e || g == f) {
                  continue;
                }
                // update paths array if there's a new route
                paths[path_length * path_count] = 0;
                paths[path_length * path_count + 1] = a;
                paths[path_length * path_count + 2] = b;
                paths[path_length * path_count + 3] = c;
                paths[path_length * path_count + 4] = d;
                paths[path_length * path_count + 5] = e;
                paths[path_length * path_count + 6] = f;
                paths[path_length * path_count + 7] = g;
                paths[path_length * path_count + 8] = 0;
                path_count++;
              }
            }
          }
        }
      }
    }
  }
}

/**
 * @brief Count how many paths there's gonna be to assign memory space
 *
 * @param cities stopes
 * @param cities_size qty of stops
 * @return int qty of different possible paths
 */
int count_paths(int *cities, int cities_size) {
  int path_count = 0;
  for (size_t i = 1; i < cities_size; i++) {
    for (size_t j = 2; j < cities_size; j++) {
      for (size_t k = 3; k < cities_size; k++) {
        for (size_t l = 4; l < cities_size; l++) {
          for (size_t m = 5; m < cities_size; m++) {
            for (size_t n = 6; n < cities_size; n++) {
              for (size_t o = 7; o < cities_size; o++) {
                path_count++;
              }
            }
          }
        }
      }
    }
  }
  return path_count;
}

/**
 * @brief Get the distance obetween 2 cities
 *
 * @param cities possible cities
 * @param cities_sizes qty of cities
 * @param distances distances matrix
 * @param c1 city 1
 * @param c2 city 2
 * @return int distance between c1 and c2
 */
int get_distance(int *cities, int cities_sizes, int *distances, int c1,
                 int c2) {
  int tc1 = c1;
  int tc2 = c2;

  // first idx always comes first
  if (c1 > c2) {
    tc1 = c2;
    tc2 = c1;
  }

  int result = 0;
  // find distance
  for (int i = 0; 1 > 0; i++) {
    if (distances[i * 3] == tc1 && distances[i * 3 + 1] == tc2) {
      result = distances[i * 3 + 2];
      // only way out
      break;
    }
  }

  // return distance
  return result;
}

/**
 * @brief obtain the distance of a given path
 *
 * @param path
 * @param cities
 * @param cities_size
 * @param distances
 * @param path_idx
 * @return int
 */
int path_length(int *path, int *cities, int cities_size, int *distances,
                int path_idx) {
  int path_length = 9;
  int result = 0;
  for (int i = 0; i < path_length - 1; i++) {
    int distance = get_distance(cities, cities_size, distances,
                                path[path_idx * path_length + i],
                                (path[path_idx * path_length + i + 1]));
    result = result + distance;
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

  /**
   * @brief cities are as follows
   * 0 san jose
   * 1 limon
   * 2 san francisco
   * 3 alajuela
   * 4 liberia
   * 5 paraiso
   * 6 puntarenas
   * 7 san isidro
   */
  int cities[] = {0, 1, 2, 3, 4, 5, 6, 7};
  int cities_size = 8;

  /**
   * @brief Fields are as follows
   * {
   *   from/to, to/from, distance un km,
   *   ...,
   *   ...
   * }
   *
   */
  // clang-format off
  int distances[] = {
    0, 1, 115,
    0, 2, 8,
    0, 3, 17,
    0, 4, 167,
    0, 5, 26,
    0, 6, 83,
    0, 7, 75,
    1, 2, 120,
    1, 3, 129,
    1, 4, 272,
    1, 5, 92,
    1, 6, 197,
    1, 7, 100,
    2, 3, 9,
    2, 4, 160,
    2, 5, 34,
    2, 6, 78,
    2, 7, 83,
    3, 4, 151,
    3, 5, 43,
    3, 6, 69,
    3, 7, 91,
    4, 5, 193,
    4, 6, 98,
    4, 7, 236,
    5, 6, 108,
    5, 7, 55,
    6, 7, 141
  };
  // clang-format on

  int *paths = NULL;
  int *local_paths = NULL;
  // calculate how many memory is needed
  int path_count = count_paths(cities, cities_size);

  // allocate such memory
  paths = malloc(sizeof(int) * path_count * (cities_size + 1));

  // obtain all possible routes
  get_paths(cities, cities_size, paths);

  local_paths = malloc(sizeof(int) * path_count / size);

  // start with very high lowest route
  int lowest = 100000000;
  // set index to 0, which will be the first to override previous result
  int lowest_idx = 0;

  // Scatter the random numbers to all processes
  MPI_Scatter(paths, path_count / size, MPI_INT, local_paths, path_count / size,
              MPI_INT, 0, MPI_COMM_WORLD);

  // obtain distances for each route and determine shortest distance
  for (int i = 0; i < path_count; i++) {
    int path_distance = path_length(paths, cities, cities_size, distances, i);
    if (path_distance < lowest) {
      lowest = path_distance;
      lowest_idx = i;
    }
  }

  // obtain all distances which match shortest and print
  for (int i = path_count / size * rank; i < path_count / size * (rank + 1);
       i++) {
    int path_distance = path_length(paths, cities, cities_size, distances, i);
    if (path_distance == lowest) {
      printf("\nShortest path found %d km at index %d rank is %d\n", lowest, i,
             rank);
      printf("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", paths[i * 9],
             paths[i * 9 + 1], paths[i * 9 + 2], paths[i * 9 + 3],
             paths[i * 9 + 4], paths[i * 9 + 5], paths[i * 9 + 6],
             paths[i * 9 + 7], paths[i * 9 + 8]);
      for (size_t j = 0; j < 8; j++) {
        printf("Distance from %d to %d is %dkm\n", paths[i * 9 + j],
               paths[i * 9 + j + 1],
               get_distance(cities, cities_size, distances, paths[i * 9 + j],
                            paths[i * 9 + j + 1]));
      }
    }
  }

  int global_min = 0;
  // obtain min from all
  MPI_Reduce(&lowest, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
  // free used memory for paths
  free(paths);
  if (rank == 0) {
    // print collected min distance
    printf("\nGlobal Min distance is %dkm\n", global_min);
  }
  ierr = MPI_Finalize();
  return 0;
}
