#include <iostream>
#include <chrono>
#include <cmath>

// CUDA kernel for matrix multiplication using shared memory
__global__ void matrixMulShared(float *A, float *B, float *C, int size) {
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int row = blockIdx.y * blockDim.y + ty;
    int col = blockIdx.x * blockDim.x + tx;

    int TILE_DIM = blockDim.x;

    // FIX: A single extern array must be declared for dynamic shared memory
    // and then partitioned with pointers to prevent overlapping data.
    extern __shared__ float s_Mem[]; 
    float* s_A = s_Mem;
    float* s_B = &s_Mem[TILE_DIM * TILE_DIM];

    float Cvalue = 0.0f;

    for (int m = 0; m < (size + TILE_DIM - 1) / TILE_DIM; ++m) {
        // Load a tile of A and B into shared memory
        int aRow = row;
        int aCol = m * TILE_DIM + tx;
        int bRow = m * TILE_DIM + ty;
        int bCol = col;

        if (aRow < size && aCol < size) {
            s_A[ty * TILE_DIM + tx] = A[aRow * size + aCol];
        } else {
            s_A[ty * TILE_DIM + tx] = 0.0f;
        }

        if (bRow < size && bCol < size) {
            s_B[ty * TILE_DIM + tx] = B[bRow * size + bCol];
        } else {
            s_B[ty * TILE_DIM + tx] = 0.0f;
        }

        __syncthreads(); // Synchronize before computing

        // Perform the matrix multiplication for the current tiles
        for (int k = 0; k < TILE_DIM; ++k) {
            Cvalue += s_A[ty * TILE_DIM + k] * s_B[k * TILE_DIM + tx];
        }

        __syncthreads(); // Synchronize before loading the next tile
    }

    if (row < size && col < size) {
        C[row * size + col] = Cvalue;
    }
}

void runMatrixMulTest(int size) {
    float *h_A = new float[size * size];
    float *h_B = new float[size * size];
    float *h_C = new float[size * size];

    // Initialize host matrices
    for (int i = 0; i < size * size; ++i) {
        h_A[i] = 1.0f;
        h_B[i] = 2.0f;
    }

    float *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size * size * sizeof(float));
    cudaMalloc(&d_B, size * size * sizeof(float));
    cudaMalloc(&d_C, size * size * sizeof(float));

    cudaMemcpy(d_A, h_A, size * size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size * size * sizeof(float), cudaMemcpyHostToDevice);

    // Grid and block config
    int blockSize = 32; 
    dim3 blockDim(blockSize, blockSize);
    dim3 gridDim((size + blockSize - 1) / blockSize, (size + blockSize - 1) / blockSize);

    // Shared memory size calculation (for 2 matrices: A and B)
    int sharedMemSize = 2 * blockSize * blockSize * sizeof(float);

    int numRuns = 20;
    double totalDuration = 0.0;

    // Execution loop for average calculation
    for (int run = 0; run < numRuns; ++run) {
        auto start = std::chrono::high_resolution_clock::now();
        matrixMulShared<<<gridDim, blockDim, sharedMemSize>>>(d_A, d_B, d_C, size);
        cudaDeviceSynchronize(); 
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;
        totalDuration += duration.count();
    }

    cudaMemcpy(h_C, d_C, size * size * sizeof(float), cudaMemcpyDeviceToHost);

    // Verify correctness 
    float expected = static_cast<float>(size) * 1.0f * 2.0f;
    bool isValid = true;
    if (std::abs(h_C[0] - expected) > 1e-3) isValid = false;
    if (std::abs(h_C[size * size - 1] - expected) > 1e-3) isValid = false;

    // Output formatting 
    double averageDuration = totalDuration / numRuns;

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(3);
    
    std::cout << "Avg Shared Exec Time [";
    std::cout.width(4); 
    std::cout << size << "x";
    std::cout.width(4); 
    std::cout << size << "] over " << numRuns << " runs : " << averageDuration << " ms ";
    
    if(isValid) {
        std::cout << "(Valid)\n";
    } else {
        std::cout << "(INVALID DATA)\n";
    }

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    delete[] h_A;
    delete[] h_B;
    delete[] h_C;
}

int main() {
    int testSizes[] = {512, 1024, 2048}; 
    int numTests = sizeof(testSizes) / sizeof(testSizes[0]);

    for (int i = 0; i < numTests; ++i) {
        runMatrixMulTest(testSizes[i]);
    }

    return 0;
}