#include "cpu_heat.h"
#include <iostream>
#include <vector>
#include <iomanip> // for std::setprecision
#include <cstdlib> // for atof

// Function to initialize the matrices
void initialize_matrices(std::vector<std::vector<float>>& previousMatrix,
                         std::vector<std::vector<float>>& nextMatrix,
                         int n, int m) {
    }

// Function to perform one heat propagation step
void heat_step(const std::vector<std::vector<float>>& previousMatrix,
               std::vector<std::vector<float>>& nextMatrix,
               int n, int m{
}

// Function to compute average temperature per row
void compute_row_averages(const std::vector<std::vector<float>>& matrix, int n, int m) {
    for (int i = 0; i < n; ++i) {
        


}

void run_cpu_simulation(int n, int m, int p, bool compute_average{}
