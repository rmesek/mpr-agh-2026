/**
 * @par Build and run
 * @code{.sh}
 * mpicc mpi_p2p_benchmark.c -o mpi_p2p_benchmark
 * mpiexec -np 2 ./mpi_p2p_benchmark
 * @endcode
 **/

#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Number of round-trip iterations for the benchmark
const int N_ITERS = 1000;

// Message sizes to benchmark (in bytes)
const int MSG_SIZES[] = {1,    4,     16,    64,     256,    1024,
                         4096, 16384, 65536, 262144, 1048576, 4194304,
                          16777216, 33554432, 67108864, 134217728};

// Send modes to benchmark
typedef int (*mpi_send_fn)(const void*, int, MPI_Datatype, int, int, MPI_Comm);
struct benchmark_mode {
  const char* name;
  mpi_send_fn func;
} MODES[] = {{"MPI_Send", MPI_Send}, {"MPI_Ssend", MPI_Ssend}};

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

  // Print hosts information
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

  // Print benchmark header
  if (my_rank == SENDER) {
    printf(
        "\nSend Mode\tMessage Size [Bytes]\tThroughput [Mbit/s]\tLatency "
        "[ms]\n");
  }

  for (size_t m = 0; m < sizeof(MODES) / sizeof(MODES[0]); m++) {
    for (size_t i = 0; i < sizeof(MSG_SIZES) / sizeof(MSG_SIZES[0]); i++) {
      int msg_size = MSG_SIZES[i];
      uint8_t* buffer = (uint8_t*)malloc(msg_size);
      if (buffer == NULL) {
        printf("Failed to allocate buffer of size %d bytes.\n", msg_size);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }

      // Synchronize all processes before starting the clock
      MPI_Barrier(MPI_COMM_WORLD);
      double start_time = MPI_Wtime();

      for (int iter = 0; iter < N_ITERS; iter++) {
        switch (my_rank) {
          case SENDER:
            // 1. Send out (Ping)
            MODES[m].func(buffer, msg_size, MPI_UINT8_T, RECEIVER, 0,
                          MPI_COMM_WORLD);
            // 4. Receive back (Pong)
            MPI_Recv(buffer, msg_size, MPI_UINT8_T, RECEIVER, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            break;
          case RECEIVER:
            // 2. Receive (Ping)
            MPI_Recv(buffer, msg_size, MPI_UINT8_T, SENDER, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            // 3. Send back (Pong)
            MODES[m].func(buffer, msg_size, MPI_UINT8_T, SENDER, 0,
                          MPI_COMM_WORLD);
            break;
        }
      }

      // Stop the clock and calculate the latency
      double end_time = MPI_Wtime();
      double one_way_latency = (end_time - start_time) / (2.0 * N_ITERS);

      if (my_rank == SENDER) {
        double throughput_mbps =
            (msg_size * 8.0) / (one_way_latency * 1e6);  // Convert to Mbit/s
        double latency_ms = one_way_latency * 1e3;       // Convert to ms
        printf("%s\t%d\t%f\t%f\n", MODES[m].name, msg_size, throughput_mbps,
               latency_ms);
      }
    }
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}