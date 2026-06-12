#include<stdio.h>
#include<stdlib.h>

#define BLOCK_SIZE 32 
#define ITERATIONS 20

__global__ void matrix_transpose_naive(int *input, int *output, int N) {

	int indexX = threadIdx.x + blockIdx.x * blockDim.x;
	int indexY = threadIdx.y + blockIdx.y * blockDim.y;
	
    if(indexX < N && indexY < N) {
	    int index = indexY * N + indexX;
	    int transposedIndex = indexX * N + indexY;

	    // this has discoalesced global memory store  
	    output[transposedIndex] = input[index];
    }
}

__global__ void matrix_transpose_shared(int *input, int *output, int N) {

	__shared__ int sharedMemory [BLOCK_SIZE] [BLOCK_SIZE + 1];

	// global index	
	int indexX = threadIdx.x + blockIdx.x * blockDim.x;
	int indexY = threadIdx.y + blockIdx.y * blockDim.y;

	// transposed global memory index
	int tindexX = threadIdx.x + blockIdx.y * blockDim.x;
	int tindexY = threadIdx.y + blockIdx.x * blockDim.y;

	// local index
	int localIndexX = threadIdx.x;
	int localIndexY = threadIdx.y;

    if(indexX < N && indexY < N) {
	    int index = indexY * N + indexX;
	    // reading from global memory in coalesed manner and performing tanspose in shared memory
	    sharedMemory[localIndexX][localIndexY] = input[index];
    }

	__syncthreads();

    if(tindexX < N && tindexY < N) {
        int transposedIndex = tindexY * N + tindexX;
	    // writing into global memory in coalesed fashion via transposed data in shared memory
	    output[transposedIndex] = sharedMemory[localIndexY][localIndexX];
    }
}

// basically just fills the array with index.
void fill_array(int *data, int N) {
	for(int idx=0;idx<(N*N);idx++)
		data[idx] = idx;
}

// Utility function to measure average kernel execution time
float measure_time(void (*kernel)(int*, int*, int), int *d_in, int *d_out, int N, dim3 gridSize, dim3 blockSize) {
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    float ms = 0, total_ms = 0;

    // Warm-up run
    kernel<<<gridSize, blockSize>>>(d_in, d_out, N);
    cudaDeviceSynchronize();

    for (int i = 0; i < ITERATIONS; i++) {
        cudaEventRecord(start);
        kernel<<<gridSize, blockSize>>>(d_in, d_out, N);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&ms, start, stop);
        total_ms += ms;
    }

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    return total_ms / ITERATIONS;
}

int main(void) {
    int sizes[] = {1024, 2048, 4096};
    
    printf("Execution Times for conflict_solved.cu (Avg %d Iterations):\n", ITERATIONS);

    for(int s = 0; s < 3; s++) {
        int N = sizes[s];
	    int size = N * N * sizeof(int);
	    int *a, *b;
        int *d_a, *d_b; 

	    // Alloc space for host copies of a, b and setup input values
	    a = (int *)malloc(size); fill_array(a, N);
	    b = (int *)malloc(size);

	    // Alloc space for device copies
	    cudaMalloc((void **)&d_a, size);
	    cudaMalloc((void **)&d_b, size);

	    // Copy inputs to device
	    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);

	    dim3 blockSize(BLOCK_SIZE, BLOCK_SIZE, 1);
	    dim3 gridSize(N/BLOCK_SIZE, N/BLOCK_SIZE, 1);

        printf("Matrix Size: %d x %d\n", N, N);

        float t_naive = measure_time(matrix_transpose_naive, d_a, d_b, N, gridSize, blockSize);
        printf(" [Ex 1.2] Naive (Global Only)     : %.3f ms\n", t_naive);

        float t_shared_solved = measure_time(matrix_transpose_shared, d_a, d_b, N, gridSize, blockSize);
        printf(" [Ex 1.2] Shared (Conflict Solved): %.3f ms\n", t_shared_solved);
        printf("----------------------------------------------------\n");

	    // terminate memories for this iteration
	    free(a); free(b);
        cudaFree(d_a); cudaFree(d_b); 
    }

	return 0;
}