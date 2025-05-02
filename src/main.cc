#include "cpu_heat.h"
#include "gpu_heat.h"
#include <cuda_runtime.h>

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cmath>

int main(int argc, char* argv[]) {
    int n = 32;
    int m = 32;
    int p = 10;
    bool compute_average = false;
    bool use_gpu = false;
    bool timing_enabled = false;
    bool skip_cpu = false;
    bool use_double = false;
    int threads_per_block = 64;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-n" && i + 1 < argc) n = std::atoi(argv[++i]);
        else if (arg == "-m" && i + 1 < argc) m = std::atoi(argv[++i]);
        else if (arg == "-p" && i + 1 < argc) p = std::atoi(argv[++i]);
        else if (arg == "-a") compute_average = true;
        else if (arg == "-g") use_gpu = true;
        else if (arg == "-t") timing_enabled = true;
        else if (arg == "-c") skip_cpu = true;
        else if (arg == "-d") use_double = true;
    }

    if (use_double) {
        std::vector<std::vector<double>> cpu_matrix, gpu_matrix;

        if (!skip_cpu) {
            auto start = std::chrono::high_resolution_clock::now();
            run_cpu_simulation<double>(n, m, p, compute_average, cpu_matrix);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            if (timing_enabled)
                std::cout << "CPU total time (double): " << duration.count() << " ms" << std::endl;
        }

        if (use_gpu) {
            cudaEvent_t start, stop;
            cudaEventCreate(&start) ;
	    cudaEventCreate(&stop);

            cudaEventRecord(start);
            run_gpu_simulation<float>(n, m, p, compute_average, gpu_matrix, timing_enabled, threads_per_block);
            cudaEventRecord(stop);
            cudaEventSynchronize(stop);

            float milliseconds = 0;
            cudaEventElapsedTime(&milliseconds, start, stop);
            std::cout << "GPU Execution Time (float): " << milliseconds << " ms" << std::endl;

            cudaEventDestroy(start);
            cudaEventDestroy(stop);
        }

        if (!skip_cpu && use_gpu) {
            float max_diff = 0.0f;
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < m; ++j)
                    max_diff = std::max(max_diff, std::abs(cpu_matrix[i][j] - gpu_matrix[i][j]));
            std::cout << "Max diff (float): " << max_diff << std::endl;
        }
    }

    return 0;
}
