#include <iostream>
#include <chrono>
#include <cublas_v2.h>
#include <cmath>

#define IDX2C(i, j, ld) (((j)*(ld))+(i)) // Column-major indexing for cuBLAS

void runCublasTest(int size) {
    // Allocate host memory
    float *h_A = new float[size * size];
    float *h_B = new float[size * size];
    float *h_C = new float[size * size];

    // Initialize host matrices (column-major for cuBLAS)
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i < size; ++i) {
            h_A[IDX2C(i, j, size)] = 1.0f;
            h_B[IDX2C(i, j, size)] = 2.0f;
        }
    }

    // Allocate device memory
    float *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size * size * sizeof(float));
    cudaMalloc(&d_B, size * size * sizeof(float));
    cudaMalloc(&d_C, size * size * sizeof(float));

    // Copy data from host to device
    cudaMemcpy(d_A, h_A, size * size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size * size * sizeof(float), cudaMemcpyHostToDevice);

    // Initialize cuBLAS
    cublasHandle_t handle;
    cublasCreate(&handle);

    float alpha = 1.0f;
    float beta = 0.0f;
    int n = size;

    // Without Warm-up (Cold Start)
    auto startCold = std::chrono::high_resolution_clock::now();
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, n, n, n, &alpha, d_A, n, d_B, n, &beta, d_C, n);
    cudaDeviceSynchronize(); 
    auto endCold = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> coldDuration = endCold - startCold;

    // With Warm-up (Average of 20 subsequent runs)
    int numRuns = 20;
    double totalWarmDuration = 0.0;

    for (int run = 0; run < numRuns; ++run) {
        auto startWarm = std::chrono::high_resolution_clock::now();
        cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, n, n, n, &alpha, d_A, n, d_B, n, &beta, d_C, n);
        cudaDeviceSynchronize();
        auto endWarm = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> warmDuration = endWarm - startWarm;
        totalWarmDuration += warmDuration.count();
    }
    double avgWarmDuration = totalWarmDuration / numRuns;

    // Output formatting
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(3);
    
    std::cout << "cuBLAS [";
    std::cout.width(4); 
    std::cout << size << "x";
    std::cout.width(4); 
    std::cout << size << "] | ";
    std::cout << "Cold Start : ";
    std::cout.width(7);
    std::cout << coldDuration.count() << " ms | ";
    std::cout << "Avg Warm (" << numRuns << " runs) : " << avgWarmDuration << " ms\n";

    // Free cuBLAS resources and memory
    cublasDestroy(handle);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

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
        runCublasTest(testSizes[i]);
    }

    return 0;
}