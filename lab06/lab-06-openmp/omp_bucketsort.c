#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PRINT_DEBUG
#define PRINT_DEBUG 1
#endif

// algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
// the state must be initialized to non-zero
uint32_t xorshift32(uint32_t* state) {
  uint32_t x = *state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return *state = x;
}

double omp_auto_xorshift32(uint32_t base_seed, uint32_t* array,
                           size_t array_size) {
#pragma omp parallel
  {
    uint32_t thread_seed = base_seed + omp_get_thread_num();
#pragma omp for schedule(auto)
    for (size_t i = 0; i < array_size; i++) {
      array[i] = xorshift32(&thread_seed);
    }
  }
}

void print_array(const uint32_t* array, size_t array_size) {
  for (size_t i = 0; i < array_size; i++) {
    printf("%u ", array[i]);
  }
  printf("\n");
}

typedef struct {
  uint32_t* elements;
  size_t size;
  size_t capacity;
} bucket_t;

void bucket_init(bucket_t* b, size_t initial_capacity) {
  b->size = 0;
  b->capacity = initial_capacity > 0 ? initial_capacity : 16;
  b->elements = malloc(b->capacity * sizeof(uint32_t));
  if (b->elements == NULL) {
    fprintf(stderr, "Failed to allocate memory for bucket elements\n");
    exit(EXIT_FAILURE);
  }
}

void bucket_append(bucket_t* b, uint32_t value) {
  if (b->size == b->capacity) {
    b->capacity *= 2;
    b->elements = realloc(b->elements, b->capacity * sizeof(uint32_t));
    if (b->elements == NULL) {
      fprintf(stderr, "Failed to reallocate memory for bucket elements\n");
      exit(EXIT_FAILURE);
    }
  }
  b->elements[b->size++] = value;
}

void bucket_free(bucket_t* b) {
  free(b->elements);
  b->elements = NULL;
  b->capacity = 0;
  b->size = 0;
}

void buckets_insert(bucket_t* buckets, size_t num_buckets,
                    uint32_t bucket_range, uint32_t value) {
  size_t bucket_index = value / bucket_range;
  if (bucket_index >= num_buckets) {
    bucket_index = num_buckets - 1;
  }
  bucket_append(&buckets[bucket_index], value);
}

void print_buckets(const bucket_t* buckets, size_t num_buckets) {
  for (size_t i = 0; i < num_buckets; i++) {
    printf("Bucket %zu: ", i);
    print_array(buckets[i].elements, buckets[i].size);
  }
}

void print_bucket_stats_v3(const size_t* bucket_offsets, size_t num_buckets) {
  if (num_buckets == 0) return;

  size_t min_size = bucket_offsets[1] - bucket_offsets[0];
  size_t max_size = min_size;
  double sum_size = 0;

  for (size_t i = 0; i < num_buckets; i++) {
    size_t sz = bucket_offsets[i + 1] - bucket_offsets[i];
    if (sz < min_size) min_size = sz;
    if (sz > max_size) max_size = sz;
    sum_size += sz;
  }

  double avg_size = sum_size / num_buckets;
  double var_size = 0;

  for (size_t i = 0; i < num_buckets; i++) {
    double diff_size = (bucket_offsets[i + 1] - bucket_offsets[i]) - avg_size;
    var_size += diff_size * diff_size;
  }

  double stddev_size = sqrt(var_size / num_buckets);

  printf("bucketsort_v3.min_elem       %zu\n", min_size);
  printf("bucketsort_v3.max_elem       %zu\n", max_size);
  printf("bucketsort_v3.avg_elem       %.2f\n", avg_size);
  printf("bucketsort_v3.stddev_elem    %.2f\n", stddev_size);
}

int compare_uint32(const void* a, const void* b) {
  uint32_t arg1 = *(const uint32_t*)a;
  uint32_t arg2 = *(const uint32_t*)b;
  if (arg1 < arg2) return -1;
  if (arg1 > arg2) return 1;
  return 0;
}

void buckets_sort(bucket_t* buckets, size_t num_buckets) {
  for (size_t i = 0; i < num_buckets; i++) {
    if (buckets[i].size > 1) {
      qsort(buckets[i].elements, buckets[i].size, sizeof(uint32_t),
            compare_uint32);
    }
  }
}

// BUCKETSORT IMPLEMENTATIONS

void bucketsort_v3(uint32_t* array, size_t array_size, size_t num_buckets) {
  uint32_t bucket_range = UINT32_MAX / num_buckets;
  int num_threads = omp_get_max_threads();

  // local buckets for each thread
  bucket_t* local_buckets =
      malloc(num_threads * num_buckets * sizeof(bucket_t));

  // offset in the input array for each bucket
  size_t* bucket_offsets = calloc(num_buckets + 1, sizeof(size_t));

#if PRINT_DEBUG
  double phase_time;
#endif

#pragma omp parallel
  {
    int thread_id = omp_get_thread_num();
    bucket_t* local_bucket = &local_buckets[thread_id * num_buckets];

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    phase_time = omp_get_wtime();
#endif

    // initialize local buckets
    for (size_t b_idx = 0; b_idx < num_buckets; b_idx++) {
      bucket_init(&local_bucket[b_idx],
                  array_size / num_buckets / num_threads + 10);
    }

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v3.init           %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

#pragma omp for schedule(auto)
    // insert elements into local buckets
    for (size_t i = 0; i < array_size; i++) {
      buckets_insert(local_bucket, num_buckets, bucket_range, array[i]);
    }

#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v3.distribution   %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

#pragma omp for schedule(auto)
    // sum up sizes of local buckets to get global bucket offsets
    for (size_t b_idx = 0; b_idx < num_buckets; b_idx++) {
      for (int t_id = 0; t_id < num_threads; t_id++) {
        bucket_t* b = &local_buckets[t_id * num_buckets + b_idx];
        bucket_offsets[b_idx + 1] += b->size;
      }
    }

#pragma omp single
    {
      // prefix sum to get offsets
      for (size_t b_idx = 1; b_idx <= num_buckets; b_idx++) {
        bucket_offsets[b_idx] += bucket_offsets[b_idx - 1];
      }
    }

#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v3.calc_offsets   %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

#pragma omp for schedule(auto)
    // merge local buckets into global buckets (store in the input array)
    for (size_t b_idx = 0; b_idx < num_buckets; b_idx++) {
      size_t write_pos = bucket_offsets[b_idx];
      for (int t_id = 0; t_id < num_threads; t_id++) {
        bucket_t* b = &local_buckets[t_id * num_buckets + b_idx];
        if (b->size > 0) {
          memcpy(&array[write_pos], b->elements, b->size * sizeof(uint32_t));
          write_pos += b->size;
        }
      }
    }

#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v3.copy_and_merge %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

#pragma omp for schedule(auto)
    // sort global buckets stored in the input array
    for (size_t b_idx = 0; b_idx < num_buckets; b_idx++) {
      qsort(&array[bucket_offsets[b_idx]],
            bucket_offsets[b_idx + 1] - bucket_offsets[b_idx], sizeof(uint32_t),
            compare_uint32);
    }

#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v3.sort           %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

#if PRINT_DEBUG
#pragma omp single
    {
      print_bucket_stats_v3(bucket_offsets, num_buckets);
    }
#endif

    // cleanup
    for (size_t b_idx = 0; b_idx < num_buckets; b_idx++) {
      bucket_free(&local_bucket[b_idx]);
    }

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v3.cleanup        %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
    }
#endif
  }  // end of #pragma omp parallel

  free(local_buckets);
  free(bucket_offsets);
}

// END OF BUCKETSORT IMPLEMENTATIONS

int main(int argc, char** argv) {
  uint32_t base_seed;
  size_t array_size;
  size_t num_buckets;

  if (argc != 4 || sscanf(argv[1], "%u", &base_seed) != 1 ||
      sscanf(argv[2], "%zu", &array_size) != 1 ||
      sscanf(argv[3], "%zu", &num_buckets) != 1) {
    fprintf(stderr, "Usage: %s <base_seed> <array_size> <num_buckets>\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  if (base_seed == 0) {
    fprintf(stderr, "base_seed must be non-zero\n");
    return EXIT_FAILURE;
  }

  uint32_t* array = malloc(array_size * sizeof(uint32_t));
  if (array == NULL) {
    fprintf(stderr, "Failed to allocate memory for array\n");
    return EXIT_FAILURE;
  }

  // BUCKETSORT V3

  // generate random numbers in parallel using OpenMP
  double start_time;
  start_time = omp_get_wtime();
  omp_auto_xorshift32(base_seed, array, array_size);
  double omp_auto_xorshift32_time_ms = (omp_get_wtime() - start_time) * 1e3;
  printf("omp_auto_xorshift32          %f ms\n", omp_auto_xorshift32_time_ms);

  // sort using bucketsort
  start_time = omp_get_wtime();
  bucketsort_v3(array, array_size, num_buckets);
  double bucketsort_v3_time_ms = (omp_get_wtime() - start_time) * 1e3;
  printf("bucketsort_v3                %f ms\n", bucketsort_v3_time_ms);

  // validate sorting result
  for (size_t i = 1; i < array_size; i++) {
    if (array[i - 1] > array[i]) {
      fprintf(stderr, "Error: array is not sorted at index %zu\n", i);
      free(array);
      return EXIT_FAILURE;
    }
  }

  // END OF BUCKETSORT V3

  // cleanup
  free(array);

  return EXIT_SUCCESS;
}