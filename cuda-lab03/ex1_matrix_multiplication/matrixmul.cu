#include <iostream>
#include <chrono>

// CUDA kernel for matrix multiplication
__global__ void matrixMul(float *A, float *B, float *C, int size) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < size && col < size) {
        float sum = 0.0f;
        for (int i = 0; i < size; ++i) {
            sum += A[row * size + i] * B[i * size + col];
        }
        C[row * size + col] = sum;
    }
}

// Function handling execution and memory logic for a specific matrix size
void runMatrixMulTest(int size) {
    // Allocate host memory
    float *h_A = new float[size * size];
    float *h_B = new float[size * size];
    float *h_C = new float[size * size];

    // Initialize host matrices
    for (int i = 0; i < size * size; ++i) {
        h_A[i] = 1.0f;
        h_B[i] = 2.0f;
    }

    // Allocate device memory
    float *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size * size * sizeof(float));
    cudaMalloc(&d_B, size * size * sizeof(float));
    cudaMalloc(&d_C, size * size * sizeof(float));

    // Copy data from host to device
    cudaMemcpy(d_A, h_A, size * size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size * size * sizeof(float), cudaMemcpyHostToDevice);

    // Define grid and block dimensions
    int blockSize = 16;
    dim3 blockDim(blockSize, blockSize);
    dim3 gridDim((size + blockSize - 1) / blockSize, (size + blockSize - 1) / blockSize);

    int numRuns = 20;
    double totalDuration = 0.0;

    // Make 20 runs to get the average execution time
    for (int run = 0; run < numRuns; ++run) {
        // Measure kernel execution time
        auto start = std::chrono::high_resolution_clock::now();
        matrixMul<<<gridDim, blockDim>>>(d_A, d_B, d_C, size);
        cudaDeviceSynchronize(); // Wait for the kernel to finish
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate duration in milliseconds and add to total
        std::chrono::duration<double, std::milli> duration = end - start;
        totalDuration += duration.count();
    }

    // Calculate average duration
    double averageDuration = totalDuration / numRuns;

    // Output using std::cout with built-in formatting methods
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(3);
    
    std::cout << "Average Execution Time [";
    std::cout.width(4); 
    std::cout << size << "x";
    std::cout.width(4); 
    std::cout << size << "] over " << numRuns << " runs : " << averageDuration << " ms\n";

    // Free device memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    // Free host memory
    delete[] h_A;
    delete[] h_B;
    delete[] h_C;
}

int main() {
    // Defines three different sizes for matrix multiplication 
    int testSizes[] = {512, 1024, 2048}; 
    int numTests = sizeof(testSizes) / sizeof(testSizes[0]);

    // Run matrix multiplication for each size
    for (int i = 0; i < numTests; ++i) {
        runMatrixMulTest(testSizes[i]);
    }

    return 0;
}