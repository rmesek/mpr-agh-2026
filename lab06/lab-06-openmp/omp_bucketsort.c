#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

void buckets_insert(bucket_t* buckets, size_t num_buckets, uint32_t bucket_range,
                   uint32_t value) {
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
      qsort(buckets[i].elements, buckets[i].size, sizeof(uint32_t), compare_uint32);
    }
  }
}

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

  // generate random numbers in parallel using OpenMP
  double start_time;
  start_time = omp_get_wtime();
  omp_auto_xorshift32(base_seed, array, array_size);
  double omp_auto_xorshift32_time_ms = (omp_get_wtime() - start_time) * 1e3;
  printf("omp_auto_xorshift32\t%f ms\n", omp_auto_xorshift32_time_ms);

  // print_array(array, array_size);

  // preallocate buckets as vectors
  uint32_t bucket_range = UINT32_MAX / num_buckets;
  bucket_t* buckets = malloc(num_buckets * sizeof(bucket_t));
  if (buckets == NULL) {
    fprintf(stderr, "Failed to allocate memory for buckets\n");
    return EXIT_FAILURE;
  }
  for (size_t i = 0; i < num_buckets; i++) {
    bucket_init(&buckets[i], array_size / num_buckets + 10);
  }

  // insert some test values into buckets
  buckets_insert(buckets, num_buckets, bucket_range, UINT32_MAX);
  buckets_insert(buckets, num_buckets, bucket_range, bucket_range);
  buckets_insert(buckets, num_buckets, bucket_range, bucket_range - 1);
  buckets_insert(buckets, num_buckets, bucket_range, 0);
  printf("\nBuckets after inserting test values:\n");
  print_buckets(buckets, num_buckets);

  // sort buckets
  buckets_sort(buckets, num_buckets);
  printf("\nBuckets after sorting:\n");
  print_buckets(buckets, num_buckets);

  // cleanup
  free(array);
  for (size_t i = 0; i < num_buckets; i++) {
    bucket_free(&buckets[i]);
  }
  free(buckets);

  return EXIT_SUCCESS;
}