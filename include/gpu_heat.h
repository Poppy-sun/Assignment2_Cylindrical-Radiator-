#ifndef GPU_HEAT_H
#define GPU_HEAT_H

#include <vector>

// Main function to run the GPU simulation
template<typename T>
void run_gpu_simulation(int n, int m, int p, bool compute_average, std::vector<std::vector<T>>& result_matrix,bool timing_enabled,int threads_per_block);

#endif // GPU_HEAT_H
