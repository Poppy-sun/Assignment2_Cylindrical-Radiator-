#ifndef CPU_HEAT_H
#define CPU_HEAT_H

#include <vector>

void initialize_matrices(std::vector<std::vector<float>>&, std::vector<std::vector<float>>&, int, int);
void heat_step(const std::vector<std::vector<float>>&, std::vector<std::vector<float>>&, int, int);
void compute_row_averages(const std::vector<std::vector<float>>&, int, int);

template<typename T>
void run_cpu_simulation(int n, int m, int p, bool compute_average, std::vector<std::vector<T>>& matrix);

#endif // CPU_HEAT_H
