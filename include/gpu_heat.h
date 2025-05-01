#ifndef GPU_HEAT_H
#define GPU_HEAT_H

#include <vector>

// Main function to run the GPU simulation
void run_gpu_simulation(int n, int m, int p, bool compute_average, std::vector<std::vector<float>>& result_matrix);

#endif // GPU_HEAT_H
