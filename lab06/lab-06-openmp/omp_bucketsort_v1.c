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
  return 0.0;
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

int compare_uint32(const void* a, const void* b) {
  uint32_t arg1 = *(const uint32_t*)a;
  uint32_t arg2 = *(const uint32_t*)b;
  if (arg1 < arg2) return -1;
  if (arg1 > arg2) return 1;
  return 0;
}

// BUCKETSORT V1 IMPLEMENTATION
void bucketsort_v1(uint32_t* array, size_t array_size, size_t num_buckets) {
  uint32_t bucket_range = UINT32_MAX / num_buckets;
  int max_threads = omp_get_max_threads();

  // Tablica przechowująca offset dla każdego wątku (od którego miejsca w
  // tablicy głównej ma zapisywać)
  size_t* thread_offsets = calloc(max_threads + 1, sizeof(size_t));

#if PRINT_DEBUG
  double phase_time;
#endif

#pragma omp parallel
  {
    int thread_id = omp_get_thread_num();
    int n_threads = omp_get_num_threads();

    // Podział kubełków pomiędzy wątki
    size_t start_bucket = (num_buckets * thread_id) / n_threads;
    size_t end_bucket = (num_buckets * (thread_id + 1)) / n_threads;
    size_t my_num_buckets = end_bucket - start_bucket;

    // Prywatne kubełki dla danego wątku
    bucket_t* my_buckets = malloc(my_num_buckets * sizeof(bucket_t));

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    phase_time = omp_get_wtime();
#endif

    // Inicjalizacja lokalnych kubełków
    for (size_t i = 0; i < my_num_buckets; i++) {
      bucket_init(&my_buckets[i], array_size / num_buckets + 10);
    }

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v1.init           %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

    // Każdy wątek czyta CAŁĄ tablicę początkową i wyłapuje tylko te liczby,
    // które wpadają do zakresu jego prywatnych kubełków.
    for (size_t i = 0; i < array_size; i++) {
      uint32_t val = array[i];
      size_t b_idx = val / bucket_range;
      if (b_idx >= num_buckets) {
        b_idx = num_buckets - 1;
      }
      if (b_idx >= start_bucket && b_idx < end_bucket) {
        bucket_append(&my_buckets[b_idx - start_bucket], val);
      }
    }

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v1.distribution   %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

    // Obliczanie łącznej wielkości danych każdego wątku
    size_t my_total_elements = 0;
    for (size_t i = 0; i < my_num_buckets; i++) {
      my_total_elements += my_buckets[i].size;
    }
    thread_offsets[thread_id + 1] = my_total_elements;

#pragma omp barrier
#pragma omp single
    {
      // Prefix sum dla offsetów wątków (obliczane przez 1 wątek)
      for (int t = 1; t <= n_threads; t++) {
        thread_offsets[t] += thread_offsets[t - 1];
      }
    }

#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v1.calc_offsets   %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

    // Każdy wątek sortuje tylko swoje kubełki
    for (size_t i = 0; i < my_num_buckets; i++) {
      if (my_buckets[i].size > 1) {
        qsort(my_buckets[i].elements, my_buckets[i].size, sizeof(uint32_t),
              compare_uint32);
      }
    }

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v1.sort           %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

    // Każdy wątek wpisuje swoje posortowane dane do unikalnego dla siebie
    // fragmentu głównej tablicy
    size_t write_pos = thread_offsets[thread_id];
    for (size_t i = 0; i < my_num_buckets; i++) {
      if (my_buckets[i].size > 0) {
        memcpy(&array[write_pos], my_buckets[i].elements,
               my_buckets[i].size * sizeof(uint32_t));
        write_pos += my_buckets[i].size;
      }
    }

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v1.copy_and_merge %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
      phase_time = omp_get_wtime();
    }
#endif

    // Czyszczenie pamięci
    for (size_t i = 0; i < my_num_buckets; i++) {
      bucket_free(&my_buckets[i]);
    }
    free(my_buckets);

#pragma omp barrier
#if PRINT_DEBUG
#pragma omp single
    {
      printf("bucketsort_v1.cleanup        %f ms\n",
             (omp_get_wtime() - phase_time) * 1e3);
    }
#endif
  }  // koniec #pragma omp parallel

  free(thread_offsets);
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

  // Generowanie losowych liczb z użyciem OpenMP
  double start_time;
  start_time = omp_get_wtime();
  omp_auto_xorshift32(base_seed, array, array_size);
  double omp_auto_xorshift32_time_ms = (omp_get_wtime() - start_time) * 1e3;
  printf("omp_auto_xorshift32          %f ms\n", omp_auto_xorshift32_time_ms);

  // Sortowanie algorytmem V1
  start_time = omp_get_wtime();
  bucketsort_v1(array, array_size, num_buckets);
  double bucketsort_v1_time_ms = (omp_get_wtime() - start_time) * 1e3;
  printf("bucketsort_v1                %f ms\n", bucketsort_v1_time_ms);

  // Walidacja wyniku
  for (size_t i = 1; i < array_size; i++) {
    if (array[i - 1] > array[i]) {
      fprintf(stderr, "Error: array is not sorted at index %zu\n", i);
      free(array);
      return EXIT_FAILURE;
    }
  }

  free(array);
  return EXIT_SUCCESS;
}