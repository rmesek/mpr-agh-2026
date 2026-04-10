#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// the state must be initialized to non-zero
uint32_t xorshift32(uint32_t* state) {
  // algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
  uint32_t x = *state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return *state = x;
}

void sequential_rand_r(unsigned int base_seed, double* array,
                       size_t array_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
  for (size_t i = 0; i < array_size; i++) {
    array[i] = rand_r(&base_seed) / (double)RAND_MAX;
  }
  end_time = omp_get_wtime();
  printf("sequential_rand_r\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_static_rand_r(unsigned int base_seed, double* array, size_t array_size,
                       int chunk_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    unsigned int thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(static, chunk_size)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = rand_r(&thread_seed) / (double)RAND_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_static_rand_r\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_dynamic_rand_r(unsigned int base_seed, double* array,
                        size_t array_size, int chunk_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    unsigned int thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(dynamic, chunk_size)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = rand_r(&thread_seed) / (double)RAND_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_dynamic_rand_r\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_guided_rand_r(unsigned int base_seed, double* array, size_t array_size,
                       int chunk_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    unsigned int thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(guided, chunk_size)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = rand_r(&thread_seed) / (double)RAND_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_guided_rand_r\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_auto_rand_r(unsigned int base_seed, double* array, size_t array_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    unsigned int thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(auto)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = rand_r(&thread_seed) / (double)RAND_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_auto_rand_r\t%f ms\n", (end_time - start_time) * 1e3);
}

void sequential_xorshift32(unsigned int base_seed, double* array,
                           size_t array_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
  uint32_t state = base_seed + 1;
  for (size_t i = 0; i < array_size; i++) {
    array[i] = xorshift32(&state) / (double)UINT32_MAX;
  }
  end_time = omp_get_wtime();
  printf("sequential_xorshift32\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_static_xorshift32(unsigned int base_seed, double* array,
                           size_t array_size, int chunk_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    uint32_t thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(static, chunk_size)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = xorshift32(&thread_seed) / (double)UINT32_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_static_xorshift32\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_dynamic_xorshift32(unsigned int base_seed, double* array,
                            size_t array_size, int chunk_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    uint32_t thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(dynamic, chunk_size)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = xorshift32(&thread_seed) / (double)UINT32_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_dynamic_xorshift32\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_guided_xorshift32(unsigned int base_seed, double* array,
                           size_t array_size, int chunk_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    uint32_t thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(guided, chunk_size)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = xorshift32(&thread_seed) / (double)UINT32_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_guided_xorshift32\t%f ms\n", (end_time - start_time) * 1e3);
}

void omp_auto_xorshift32(unsigned int base_seed, double* array,
                         size_t array_size) {
  double start_time, end_time;

  start_time = omp_get_wtime();
#pragma omp parallel
  {
    uint32_t thread_seed = base_seed + omp_get_thread_num() + 1;
#pragma omp for schedule(auto)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = xorshift32(&thread_seed) / (double)UINT32_MAX;
    }
  }
  end_time = omp_get_wtime();
  printf("omp_auto_xorshift32\t%f ms\n", (end_time - start_time) * 1e3);
}

int main(int argc, char** argv) {
  unsigned int base_seed = (unsigned int)time(NULL);
  size_t array_size;
  int chunk_size = 1024;
  double* array;

  if (argc < 2 || argc > 3 || sscanf(argv[1], "%zu", &array_size) != 1 ||
      (argc == 3 && sscanf(argv[2], "%d", &chunk_size) != 1)) {
    fprintf(stderr, "Usage: %s <array_size> [chunk]\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (chunk_size <= 0) {
    fprintf(stderr, "Chunk size must be a positive integer\n");
    return EXIT_FAILURE;
  }

  array = malloc(array_size * sizeof(double));
  if (array == NULL) {
    fprintf(stderr, "Failed to allocate memory for array\n");
    return EXIT_FAILURE;
  }

  // benchmark random number generation
  printf("benchmarking... (array_size=%zu, chunk_size=%d, max_threads=%d)\n",
         array_size, chunk_size, omp_get_max_threads());
  printf("function\ttime [ms]\n");

  sequential_rand_r(base_seed, array, array_size);

  omp_static_rand_r(base_seed, array, array_size, chunk_size);
  omp_dynamic_rand_r(base_seed, array, array_size, chunk_size);
  omp_guided_rand_r(base_seed, array, array_size, chunk_size);
  omp_auto_rand_r(base_seed, array, array_size);

  sequential_xorshift32(base_seed, array, array_size);

  omp_static_xorshift32(base_seed, array, array_size, chunk_size);
  omp_dynamic_xorshift32(base_seed, array, array_size, chunk_size);
  omp_guided_xorshift32(base_seed, array, array_size, chunk_size);
  omp_auto_xorshift32(base_seed, array, array_size);

  free(array);
  return EXIT_SUCCESS;
}