/**
 * @author RookieHPC
 * @brief Original source code at
 * https://rookiehpc.org/mpi/docs/mpi_send/index.html
 * @par Build and run
 * @code{.sh}
 * mpicc mpi_send_benchmark.c -o mpi_send_benchmark
 * mpiexec -np 2 ./mpi_send_benchmark
 * @endcode
 **/

#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Number of round-trip iterations for the benchmark
const int N_ITERS = 10000;
// Token to be sent between processes
uint8_t token = 64;

/**
 * @brief Illustrates how to send a message in a blocking fashion.
 * @details This program is meant to be run with 2 processes: a sender and a
 * receiver.
 **/
int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  // Get the number of processes and check only 2 processes are used
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size != 2) {
    printf("This application is meant to be run with 2 processes.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Get my rank and do the corresponding job
  enum role_ranks { SENDER, RECEIVER };
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // Print host information
  char hostname[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(hostname, &name_len);
  switch (my_rank) {
    case SENDER:
      printf("Process %d (sender) is running on %s\n", my_rank, hostname);
      break;
    case RECEIVER:
      printf("Process %d (receiver) is running on %s\n", my_rank, hostname);
      break;
  }

  // Synchronize all processes before starting the clock
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  for (int i = 0; i < N_ITERS; i++) {
    switch (my_rank) {
      case SENDER:
        // 1. Send out (Ping)
        MPI_Send(&token, 1, MPI_UINT8_T, RECEIVER, 0, MPI_COMM_WORLD);
        // 4. Receive back (Pong)
        MPI_Recv(&token, 1, MPI_UINT8_T, RECEIVER, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        break;

      case RECEIVER:
        // 2. Receive (Ping)
        MPI_Recv(&token, 1, MPI_UINT8_T, SENDER, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        // 3. Send back (Pong)
        MPI_Send(&token, 1, MPI_UINT8_T, SENDER, 0, MPI_COMM_WORLD);
        break;
    }
  }

  // Stop the clock and calculate the latency
  double end_time = MPI_Wtime();
  double one_way_latency = (end_time - start_time) / (2.0 * N_ITERS);

  if (my_rank == SENDER) {
    printf("Total time for %d round-trips: %.9f s\n", N_ITERS,
           end_time - start_time);
    printf("Estimated one-way latency: %.9f s (%.3f us)\n", one_way_latency,
           one_way_latency * 1e6);
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
