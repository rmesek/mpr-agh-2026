/**
 * @author RookieHPC
 * @brief Original source code at
 * https://rookiehpc.org/mpi/exercises/exercise_2/index.html
 **/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { MODE_STANDARD = 0, MODE_BUFFERED = 1 } SendMode;

/**
 * @brief Runs ping-pong benchmark for standard and buffered MPI communication.
 **/
int main(int argc, char* argv[]) {
  const int iterations = 10000;
  const int message_sizes[] = {1, 8, 64, 512, 4096, 32768, 262144, 1048576};
  const int size_count =
      (int)(sizeof(message_sizes) / sizeof(message_sizes[0]));

  // 1) Tell MPI to start
  MPI_Init(&argc, &argv);

  // 2) Check that the application is run with 2 MPI processes
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  if (comm_size != 2) {
    printf("This application must be run with 2 MPI processes, not %d.\n",
           comm_size);
    MPI_Abort(MPI_COMM_WORLD, -1);
  }

  // 3) Get my rank
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // 3.5) Get the name of the processor I am running on
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  printf("[%s] Process %d of %d is running.\n", processor_name, my_rank + 1,
         comm_size);

  // 4) Allocate send and receive buffers
  int max_size = message_sizes[size_count - 1];
  char* send_buffer = (char*)malloc((size_t)max_size);
  char* recv_buffer = (char*)malloc((size_t)max_size);
  if (send_buffer == NULL || recv_buffer == NULL) {
    fprintf(stderr, "[%s] Memory allocation failed.\n", processor_name);
    MPI_Abort(MPI_COMM_WORLD, -2);
  }
  memset(send_buffer, 0x2A, (size_t)max_size);

  if (my_rank == 0) {
    printf("Ping-pong benchmark with %d iterations.\n\n", iterations);
  }

  for (SendMode mode = MODE_STANDARD; mode <= MODE_BUFFERED; mode++) {
    const char* mode_name;
    switch (mode) {
      case MODE_STANDARD:
        mode_name = "MPI_Send";
        break;
      case MODE_BUFFERED:
        mode_name = "MPI_Bsend";
        break;
    }
    double estimated_latency_us = 0.0;

    if (my_rank == 0) {
      printf("=== %s ===\n", mode_name);
      printf("size[B]\tone-way[us]\n");
    }

    for (int i = 0; i < size_count; i++) {
      int msg_size = message_sizes[i];
      int bsend_pack_size = 0;
      char* bsend_attached = NULL;

      if (mode == MODE_BUFFERED) {
        MPI_Pack_size(msg_size, MPI_CHAR, MPI_COMM_WORLD, &bsend_pack_size);
        bsend_attached =
            (char*)malloc((size_t)(bsend_pack_size + MPI_BSEND_OVERHEAD));
        if (bsend_attached == NULL) {
          fprintf(stderr, "[%s] MPI_Bsend buffer allocation failed.\n",
                  processor_name);
          MPI_Abort(MPI_COMM_WORLD, -3);
        }
        MPI_Buffer_attach(bsend_attached, bsend_pack_size + MPI_BSEND_OVERHEAD);
      }

      // Ensure both processes are ready before measurement starts.
      MPI_Barrier(MPI_COMM_WORLD);

      double t_start = 0.0;
      double t_end = 0.0;

      if (my_rank == 0) {
        t_start = MPI_Wtime();
        for (int it = 0; it < iterations; it++) {
          switch (mode) {
            case MODE_STANDARD:
              MPI_Send(send_buffer, msg_size, MPI_CHAR, 1, 100 + i,
                       MPI_COMM_WORLD);
              break;
            case MODE_BUFFERED:
              MPI_Bsend(send_buffer, msg_size, MPI_CHAR, 1, 100 + i,
                        MPI_COMM_WORLD);
              break;
          }
          MPI_Recv(recv_buffer, msg_size, MPI_CHAR, 1, 200 + i, MPI_COMM_WORLD,
                   MPI_STATUS_IGNORE);
        }
        t_end = MPI_Wtime();
      } else {
        for (int it = 0; it < iterations; it++) {
          MPI_Recv(recv_buffer, msg_size, MPI_CHAR, 0, 100 + i, MPI_COMM_WORLD,
                   MPI_STATUS_IGNORE);
          switch (mode) {
            case MODE_STANDARD:
              MPI_Send(send_buffer, msg_size, MPI_CHAR, 0, 200 + i,
                       MPI_COMM_WORLD);
              break;
            case MODE_BUFFERED:
              MPI_Bsend(send_buffer, msg_size, MPI_CHAR, 0, 200 + i,
                        MPI_COMM_WORLD);
              break;
          }
        }
      }

      if (my_rank == 0) {
        double total = t_end - t_start;
        double one_way = total / (2.0 * (double)iterations);
        double one_way_us = one_way * 1e6;
        if (i == 0) {
          estimated_latency_us = one_way_us;
        }
        printf("%d\t%.3f\n", msg_size, one_way_us);
      }

      if (mode == MODE_BUFFERED) {
        int detached_size = 0;
        void* detached_buffer = NULL;
        MPI_Buffer_detach(&detached_buffer, &detached_size);
        free(detached_buffer);
      }
    }

    if (my_rank == 0) {
      printf("Estimated latency (smallest message): %.3f us\n\n",
             estimated_latency_us);
    }
  }

  free(send_buffer);
  free(recv_buffer);

  // 6) Tell MPI to stop
  MPI_Finalize();

  return 0;
}