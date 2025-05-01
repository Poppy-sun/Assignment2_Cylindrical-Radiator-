#include "cpu_heat.h"
#include "gpu_heat.h"
#include <cuda_runtime.h>

#include <iostream>
#include <string>
#include <vector>
#include <chrono> // For CPU timing
#include <cstdlib> // For atoi

int main(int argc, char* argv[]) {
    int n = 32;
    int m = 32;
    int p = 10;
    bool compute_average = false;
    bool use_gpu = false;

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
     if (use_gpu) {
        // --- GPU Execution ---
        std::vector<std::vector<float>> gpu_result;

        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaEventRecord(start);

        run_gpu_simulation(n, m, p, compute_average, gpu_result);

        cudaEventRecord(stop);
        cudaEventSynchronize(stop);

        float milliseconds = 0;
        cudaEventElapsedTime(&milliseconds, start, stop);

        std::cout << "GPU Execution Time: " << milliseconds << " ms" << std::endl;

        cudaEventDestroy(start);
        cudaEventDestroy(stop);

    } else {
        // --- CPU Execution ---
        auto start = std::chrono::high_resolution_clock::now();

        run_cpu_simulation(n, m, p, compute_average);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "CPU Execution Time: " << duration.count() << " ms" << std::endl;
    }

    return 0;
}
