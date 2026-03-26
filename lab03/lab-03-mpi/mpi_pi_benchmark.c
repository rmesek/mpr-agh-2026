/**
 * @brief Monte Carlo PI estimation benchmark using MPI.
 * @par Build and run
 * @code{.sh}
 * mpicc mpi_pi_benchmark.c -o mpi_pi_benchmark
 * mpiexec -np 2 ./mpi_pi_benchmark 1e10
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
    double x = (double)rand() / RAND_MAX;
    double y = (double)rand() / RAND_MAX;
    if (x * x + y * y <= 1.0) count++;
  }
  return count;
}

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (argc < 2) {
    if (my_rank == 0) {
      fprintf(stderr, "Usage: %s <n_points_per_process> [--no-header]\n",
              argv[0]);
      fprintf(stderr, "Example: %s 1e6\n", argv[0]);
    }
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  bool print_header = true;
  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--no-header") == 0) print_header = false;
  }

  uint64_t n_points_per_process = (uint64_t)strtod(argv[1], NULL);

  // Initialize random seed differently for each process
  srand(time(NULL) + my_rank);

  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();
  MPI_Barrier(MPI_COMM_WORLD);

  uint64_t local_count = count_points_in_circle(n_points_per_process);
  uint64_t global_count = 0;

  MPI_Reduce(&local_count, &global_count, 1, MPI_UINT64_T, MPI_SUM, 0,
             MPI_COMM_WORLD);

  double end_time = MPI_Wtime();

  if (my_rank == 0) {
    double pi_estimate = 4.0 * global_count / (n_points_per_process * size);
    double time_ms = (end_time - start_time) * 1e3;
    if (print_header) {
      printf("Processes\tPoints per Process\tEstimated PI\tTime [ms]\n");
    }
    printf("%d\t\t%.1e\t\t%.6f\t\t%.2f\n", size, (double)n_points_per_process,
           pi_estimate, time_ms);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
