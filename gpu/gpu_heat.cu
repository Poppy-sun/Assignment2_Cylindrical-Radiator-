#include "gpu_heat.h"
#include <cuda_runtime.h>
#include <iostream>
#include <iomanip>


__global__ void gpu_heat_step(const float* d_prev, float* d_next, int n, int m);
__global__ void gpu_compute_row_averages(const float* d_matrix, float* d_averages, int n, int m);



__global__ void gpu_heat_step(const float* d_prev, float* d_next, int n, int m) {
    int row = blockIdx.x;
    int col = threadIdx.x;

    if (row < n && col < m && col >= 1) { // col>=1, because column 0 is boundary
        int jm2 = (col - 2 + m) % m;
        int jm1 = (col - 1 + m) % m;
        int jp1 = (col + 1) % m;
        int jp2 = (col + 2) % m;

        d_next[row * m + col] = (1.60f * d_prev[row * m + jm2] +
                                 1.55f * d_prev[row * m + jm1] +
                                 1.00f * d_prev[row * m + col] +
                                 0.60f * d_prev[row * m + jp1] +
                                 0.25f * d_prev[row * m + jp2]) / 5.0f;
    }
}
__global__ void gpu_compute_row_averages(const float* d_matrix, float* d_averages, int n, int m) {
    int row = blockIdx.x;

    if (row < n) {
        float sum = 0.0f;
        for (int j = 0; j < m; ++j) {
            sum += d_matrix[row * m + j];
        }
        d_averages[row] = sum / (float)m;
    }
}
void run_gpu_simulation(int n, int m, int p, bool compute_average, std::vector<std::vector<float>>& result_matrix,bool timing_enabled, int threads_per_block) {
    // Allocate host matrices
    std::vector<float> h_previousMatrix(n * m, 0.0f);
    std::vector<float> h_nextMatrix(n * m, 0.0f);

    // Initialize host matrices (same as CPU initialization)
    for (int i = 0; i < n; ++i) {
        float boundary_value = 0.98f * (float)((i + 1) * (i + 1)) / (float)(n * n);
        h_previousMatrix[i * m + 0] = boundary_value;
        h_nextMatrix[i * m + 0] = boundary_value;
        for (int j = 1; j < m; ++j) {
            float init_value = boundary_value * (float)((m - j) * (m - j)) / (float)(m * m);
            h_previousMatrix[i * m + j] = init_value;
            h_nextMatrix[i * m + j] = init_value;
        }
    }

    // Device pointers
    float* d_prev;
    float* d_next;

    cudaMalloc(&d_prev, n * m * sizeof(float));
    cudaMalloc(&d_next, n * m * sizeof(float));

    // Copy host to device
    cudaMemcpy(d_prev, h_previousMatrix.data(), n * m * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_next, h_nextMatrix.data(), n * m * sizeof(float), cudaMemcpyHostToDevice);

    // Setup execution config
    dim3 gridDim(n);
    dim3 blockDim(threads_per_block);

    // Run p iterations
    for (int iter = 0; iter < p; ++iter) {
        gpu_heat_step<<<gridDim, blockDim>>>(d_prev, d_next, n, m);
        cudaDeviceSynchronize();
        // Swap pointers
        std::swap(d_prev, d_next);
    }

    // Copy result back
    cudaMemcpy(h_previousMatrix.data(), d_prev, n * m * sizeof(float), cudaMemcpyDeviceToHost);

    // Optional: compute row averages if needed
    if (compute_average) {
        float* d_averages;
        float* h_averages = new float[n];

        cudaMalloc(&d_averages, n * sizeof(float));
        gpu_compute_row_averages<<<gridDim, 1>>>(d_prev, d_averages, n, m);
        cudaMemcpy(h_averages, d_averages, n * sizeof(float), cudaMemcpyDeviceToHost);

        for (int i = 0; i < n; ++i) {
        std::cout << "Row " << i << " average temperature: "
          << std::fixed << std::setprecision(6) << h_averages[i] << std::endl;

        }

        delete[] h_averages;
        cudaFree(d_averages);
    }

    // Fill result matrix to return
    result_matrix.resize(n, std::vector<float>(m, 0.0f));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            result_matrix[i][j] = h_previousMatrix[i * m + j];
        }
    }

    // Free device memory
    cudaFree(d_prev);
    cudaFree(d_next);
}
