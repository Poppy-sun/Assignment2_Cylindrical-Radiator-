#include "gpu_heat.h"
#include <cuda_runtime.h>
#include <iostream>
#include <iomanip>
#include <cuda.h>
#include <type_traits>
#include <vector>


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
     __shared__ float sum;
 if (threadIdx.x == 0) sum = 0.0f;
    __syncthreads();

    if (row < n && threadIdx.x < m) {
        atomicAdd(reinterpret_cast<unsigned long long*>(&sum),
          __double_as_longlong(d_matrix[row * m + threadIdx.x]));

    }
    __syncthreads();

    if (threadIdx.x == 0) {
        d_averages[row] = sum / m;
    }
}
 

// Host function template

template <typename T>
void run_gpu_simulation(int n, int m, int p, bool compute_average,
                        std::vector<std::vector<T>>& result_matrix,
                        bool timing_enabled, int threads_per_block) {

    size_t matrix_size = n * m * sizeof(T);
    T* d_prev;
    T* d_next;
    cudaMalloc(&d_prev, matrix_size);
    cudaMalloc(&d_next, matrix_size);

    std::vector<T> h_initial(n * m, static_cast<T>(273));
    for (int i = 0; i < n; ++i) {
        h_initial[i * m + 0] = static_cast<T>(373);
    }
    cudaMemcpy(d_prev, h_initial.data(), matrix_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_next, h_initial.data(), matrix_size, cudaMemcpyHostToDevice);

    for (int iter = 0; iter < p; ++iter) {
        if constexpr (std::is_same<T, float>::value) {
            gpu_heat_step_float<<<n, threads_per_block>>>(d_prev, d_next, n, m);
        } else {
            gpu_heat_step_double<<<n, threads_per_block>>>(d_prev, d_next, n, m);
        }
        std::swap(d_prev, d_next);
    }

    std::vector<T> h_result(n * m);
    cudaMemcpy(h_result.data(), d_prev, matrix_size, cudaMemcpyDeviceToHost);

    result_matrix.resize(n, std::vector<T>(m, static_cast<T>(0)));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            result_matrix[i][j] = h_result[i * m + j];

    if (compute_average) {
        T* d_averages;
        std::vector<T> h_averages(n);
        cudaMalloc(&d_averages, n * sizeof(T));
        if constexpr (std::is_same<T, float>::value) {
            gpu_compute_row_averages_float<<<n, m>>>(d_prev, d_averages, n, m);
        } else {
            gpu_compute_row_averages_double<<<n, m>>>(d_prev, d_averages, n, m);
        }
        cudaMemcpy(h_averages.data(), d_averages, n * sizeof(T), cudaMemcpyDeviceToHost);
        for (int i = 0; i < n; ++i) {
            std::cout << "Row " << i << " average temperature: " << std::setprecision(6) << h_averages[i] << std::endl;
        }
        cudaFree(d_averages);
    }

    cudaFree(d_prev);
    cudaFree(d_next);
}

// Explicit instantiations

template void run_gpu_simulation<float>(int, int, int, bool,
        std::vector<std::vector<float>>&, bool, int);
template void run_gpu_simulation<double>(int, int, int, bool,
        std::vector<std::vector<double>>&, bool, int);
