#include "cpu_heat.h"
#include "gpu_heat.h"
#include <cuda_runtime.h>

#include <iostream>
#include <string>
#include <vector>
#include <chrono> // For CPU timing
#include <cstdlib> // For atoi
#include <cmath>
int main(int argc, char* argv[]) {
    int n = 32;
    int m = 32;
    int p = 10;
    bool compute_average = false;
    bool use_gpu = false;
    bool timing_enabled = false;
    bool skip_cpu=false;
    float max_difference=0.0f;
    int threads_per_block = 128 ;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-n" && i + 1 < argc) {
            n = std::atoi(argv[++i]);
        } else if (arg == "-m" && i + 1 < argc) {
            m = std::atoi(argv[++i]);
        } else if (arg == "-p" && i + 1 < argc) {
            p = std::atoi(argv[++i]);
        } else if (arg == "-a") {
            compute_average = true;
        } else if (arg == "-g") {
            use_gpu = true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

std::vector<std::vector<float>> cpu_result;
    std::vector<std::vector<float>> gpu_result;

    if (!use_gpu || !skip_cpu) {
        cpu_result.resize(n, std::vector<float>(m));

        auto start = std::chrono::high_resolution_clock::now();
        run_cpu_simulation(n, m, p, compute_average, cpu_result);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;

        if (timing_enabled) {
            std::cout << "CPU total time: " << duration.count() << " ms" << std::endl;
        }

        std::cout << "CPU Execution Time: " << duration.count() << " ms" << std::endl;
    }

    if (use_gpu) {
        // --- GPU Execution ---

        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaEventRecord(start);

        run_gpu_simulation(n, m, p, compute_average, gpu_result,timing_enabled, threads_per_block);

        cudaEventRecord(stop);
        cudaEventSynchronize(stop);

        float milliseconds = 0.0f;
        cudaEventElapsedTime(&milliseconds, start, stop);

        if (timing_enabled) {
        std::cout << "GPU total time (with memcpy): " << milliseconds << " ms" << std::endl;
    }

        std::cout << "GPU Execution Time: " << milliseconds << " ms" << std::endl;

        cudaEventDestroy(start);
        cudaEventDestroy(stop);

       }

    // Compare CPU and GPU results
    if (use_gpu && !skip_cpu) {
        float max_diff = 0.0f;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                float diff = std::fabs(cpu_result[i][j] - gpu_result[i][j]);
                if (diff > max_diff) max_diff = diff;
            }
        }

        std::cout << "Max difference between CPU and GPU results: " << max_diff << std::endl;

        if (max_diff > 1e-4) {
            std::cout << "⚠️ Warning: result mismatch exceeds 1e-4 threshold!" << std::endl;
        } else {
            std::cout << "✅ GPU results validated against CPU. Difference within tolerance." << std::endl;
        }
    }

    return 0;
}
