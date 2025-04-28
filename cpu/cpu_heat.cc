#include "cpu_heat.h"
#include <iostream>
#include <vector>
#include <iomanip> // for std::setprecision
#include <cstdlib> // for atof

// Function to initialize the matrices
void initialize_matrices(std::vector<std::vector<float>>& previousMatrix,
                         std::vector<std::vector<float>>& nextMatrix,
   			 int n, int m) {
// Initialize both matrices
    for (int i = 0; i < n; ++i) {
        float boundary_value = 0.98f * (float)((i + 1) * (i + 1)) / (float)(n * n);
        previousMatrix[i][0] = boundary_value;
        nextMatrix[i][0] = boundary_value;

        for (int j = 1; j < m; ++j) {
            float init_value = boundary_value * (float)((m - j) * (m - j)) / (float)(m * m);
            previousMatrix[i][j] = init_value;
            nextMatrix[i][j] = init_value;
        }
    }   
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
