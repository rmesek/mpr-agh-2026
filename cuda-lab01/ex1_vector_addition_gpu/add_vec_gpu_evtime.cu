#include<stdio.h>
#include<stdlib.h>

#define MAX_N 1048576      //... 512, 1024, ... 16384, 32768, 65536, 131072

void host_add(int *a, int *b, int *c, int n) {
  for(int idx=0;idx<n;idx++)
    c[idx] = a[idx] + b[idx];
}

__global__ void device_add(int *a, int *b, int *c, int n) {
  int index = threadIdx.x + blockIdx.x * blockDim.x;
        // Added boundary check to handle variable sizes
        if (index < n) {
            c[index] = a[index] + b[index];
        }
}

// basically just fills the array with index.
void fill_array(int *data, int n) {
  for(int idx=0;idx<n;idx++)
    data[idx] = idx;
}

void print_output(int *a, int *b, int*c, int n) {
  for(int idx=0;idx<n;idx++)
    printf("\n %d + %d  = %d",  a[idx] , b[idx], c[idx]);
}

int main(void) {
  int *a, *b, *c;
        int *d_a, *d_b, *d_c; // device copies of a, b, c
  int threads_per_block=0, no_of_blocks=0;
        // GpuTimer timer; // Commented out to prevent compilation error if gputimer.h is missing

  int size = MAX_N * sizeof(int);

  // Alloc space for host copies of a, b, c and setup input values
  a = (int *)malloc(size); fill_array(a, MAX_N);
  b = (int *)malloc(size); fill_array(b, MAX_N);
  c = (int *)malloc(size);

        // initialize a and b arrays on the host
        for (int i = 0; i < MAX_N; i++) {
           a[i] = 3;
           b[i] = 5;
        }

        // Alloc space for device copies of a, b, c
        cudaMalloc((void **)&d_a, size);
        cudaMalloc((void **)&d_b, size);
        cudaMalloc((void **)&d_c, size);

       // Copy inputs to device
        cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
        cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

        cudaEvent_t start, stop;
        // Create events
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        int num_runs = 20;

        printf("(N = %d)\n", MAX_N);
        printf("Block_Size, Avg_Elapsed_Time_ms\n");
        int block_sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
        
        for (int i = 0; i < 11; i++) {
            threads_per_block = block_sizes[i];
            no_of_blocks = (MAX_N + threads_per_block - 1) / threads_per_block;
            float total_time_ms = 0;

            for(int run = 0; run < num_runs; run++) {
                // Record start time
                cudaEventRecord(start, 0);  
                
                device_add<<<no_of_blocks,threads_per_block>>>(d_a,d_b,d_c, MAX_N);
                
                // Wait for GPU to finish before accessing on host
                cudaDeviceSynchronize();
                
                // Record stop time
                cudaEventRecord(stop, 0);
                cudaEventSynchronize(stop);

                float milliseconds = 0;
                cudaEventElapsedTime(&milliseconds, start, stop);
                total_time_ms += milliseconds;
            }
            printf("%d, %f\n", threads_per_block, total_time_ms / num_runs);
        }

        printf("\n(Block size = 256)\n");
        printf("Vector_Length, Avg_Elapsed_Time_ms\n");
        threads_per_block = 256;

        for (int n = 1024; n <= MAX_N; n *= 2) {
            no_of_blocks = (n + threads_per_block - 1) / threads_per_block;
            float total_time_ms = 0;

            for(int run = 0; run < num_runs; run++) {
                // Record start time
                cudaEventRecord(start, 0);  
                
                device_add<<<no_of_blocks,threads_per_block>>>(d_a,d_b,d_c, n);
                
                // Wait for GPU to finish before accessing on host
                cudaDeviceSynchronize();
                
                // Record stop time
                cudaEventRecord(stop, 0);
                cudaEventSynchronize(stop);

                float milliseconds = 0;
                cudaEventElapsedTime(&milliseconds, start, stop);
                total_time_ms += milliseconds;
            }
            printf("%d, %f\n", n, total_time_ms / num_runs);
        }

        // Copy result back to host
        cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);

        for (int i = 0; i < MAX_N; i++) {
            if (c[i] != 8) { // 3 + 5 = 8
                printf("Error at index %d! Expected 8, got %d\n", i, c[i]);
                break;
            }
        }

        printf("\n(CPU)\n");
        printf("Vector_Length, Avg_Elapsed_Time_ms\n");

        for (int n = 1024; n <= MAX_N; n *= 2) {
            float total_time_ms = 0;

            for(int run = 0; run < num_runs; run++) {
                // Record start time
                cudaEventRecord(start, 0);  
                
                host_add(a, b, c, n);
                
                // Record stop time
                cudaEventRecord(stop, 0);
                cudaEventSynchronize(stop);

                float milliseconds = 0;
                cudaEventElapsedTime(&milliseconds, start, stop);
                total_time_ms += milliseconds;
            }
            printf("%d, %f\n", n, total_time_ms / num_runs);
        }

        printf("\n(Testing Block Size = 2048)\n");
        
        threads_per_block = 2048;
        no_of_blocks = (MAX_N + threads_per_block - 1) / threads_per_block;

        // Clear device memory to 0 so we don't accidentally copy back previous correct answers
        cudaMemset(d_c, 0, size);

        // Clear previous errors just in case
        cudaGetLastError(); 

        printf("Attempting to launch kernel with %d threads per block...\n", threads_per_block);
        
        // Launch the kernel
        device_add<<<no_of_blocks, threads_per_block>>>(d_a, d_b, d_c, MAX_N);
        cudaDeviceSynchronize();

        // Catch and print the error IMMEDIATELY (before cudaMemcpy resets the error state)
        cudaError_t err = cudaGetLastError();
        if (err != cudaSuccess) {
            printf("CUDA Error Caught: %s\n", cudaGetErrorString(err));
        } else {
            printf("Kernel launched successfully (This shouldn't happen!)\n");
        }

        // Copy result back to host
        cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);

        for (int i = 0; i < MAX_N; i++) {
            if (c[i] != 8) { // 3 + 5 = 8
                printf("Error at index %d! Expected 8, got %d\n", i, c[i]);
                break;
            }
        }

  //print_output(a,b,c, MAX_N);

  free(a); free(b); free(c);
        cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);

  return 0;
}