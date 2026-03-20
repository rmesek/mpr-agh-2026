/**
 * @brief Monte Carlo PI estimation benchmark using MPI.
 * @par Build and run
 * @code{.sh}
 * mpicc mpi_pi_benchmark.c -o mpi_pi_benchmark
 * mpiexec -np 2 ./mpi_pi_benchmark
 * @endcode
 **/

#include <mpi.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

uint64_t count_points_in_circle(uint64_t n_points) {
  uint64_t count = 0;
  for (uint64_t i = 0; i < n_points; i++) {
    float x = (float)rand() / RAND_MAX;
    float y = (float)rand() / RAND_MAX;
    if (x * x + y * y <= 1.0) count++;
  }
  return count;
}

const uint64_t N_POINTS = 12e6;  // should be 12e10
enum scaling_mode { AMDALH, GUSTAFSON };

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <scaling_mode>\n", argv[0]);
    fprintf(stderr,
            "scaling_mode: 0 for Amdahl's law, 1 for Gustafson's law\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  enum scaling_mode mode = atoi(argv[1]);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // Initialize random seed differently for each process
  srand(time(NULL) * my_rank);

  uint64_t n_points_per_process;
  switch (mode) {
    case AMDALH:
      n_points_per_process = N_POINTS / size;
      if (my_rank == 0) printf("Using Amdahl's law scaling mode\n");
      break;
    case GUSTAFSON:
      n_points_per_process = N_POINTS / 12;
      if (my_rank == 0) printf("Using Gustafson's law scaling mode\n");
      break;
  }

  if (my_rank == 0) {
    printf("Points per process = %.1e (%.1e total)\n",
           (double)n_points_per_process, (double)n_points_per_process * size);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  uint64_t local_count = count_points_in_circle(n_points_per_process);
  uint64_t global_count = 0;

  MPI_Reduce(&local_count, &global_count, 1, MPI_UINT64_T, MPI_SUM, 0,
             MPI_COMM_WORLD);

  double end_time = MPI_Wtime();

  if (my_rank == 0) {
    double pi_estimate = 4.0 * global_count / (n_points_per_process * size);
    printf("Estimated PI = %.6f (in %.2f ms)\n", pi_estimate,
           (end_time - start_time) * 1e3);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
