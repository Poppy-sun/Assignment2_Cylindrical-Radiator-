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
               int n, int m)
{
	 #pragma omp parallel for collapse(2)
 for (int i = 0; i < n; ++i) {
        for (int j = 1; j < m; ++j) { // skip column 0 (boundary)
            int jm2 = (j - 2 + m) % m; // wrap around
            int jm1 = (j - 1 + m) % m;
            int jp1 = (j + 1) % m;
            int jp2 = (j + 2) % m;

            nextMatrix[i][j] = (1.60f * previousMatrix[i][jm2] +
                                1.55f * previousMatrix[i][jm1] +
                                1.0f  * previousMatrix[i][j] +
                                0.60f * previousMatrix[i][jp1] +
                                0.25f * previousMatrix[i][jp2]) / 5.0f;
        }
    }
	      
 }

// Function to compute average temperature per row
void compute_row_averages(const std::vector<std::vector<float>>& matrix, int n, int m) {
    
       for (int i = 0; i < n; ++i) {
        float sum = 0.0f;
        for (int j = 0; j < m; ++j) {
            sum += matrix[i][j];
        }
        float average = sum / (float)m;
        std::cout << "Row " << i << " average temperature: "
                  << std::fixed << std::setprecision(6) << average << std::endl;
    } 
}

void run_cpu_simulation(int n, int m, int p, bool compute_average,std::vector<std::vector<float>>& matrix)	{
		// Create two matrices
    std::vector<std::vector<float>> previousMatrix(n, std::vector<float>(m, 0.0f));
    std::vector<std::vector<float>> nextMatrix(n, std::vector<float>(m, 0.0f));

    // Initialize them
    initialize_matrices(previousMatrix, nextMatrix, n, m);

    // Perform p iterations
    for (int iter = 0; iter < p; ++iter) {
        heat_step(previousMatrix, nextMatrix, n, m);

        // Swap matrices (next becomes previous for next iteration)
        previousMatrix.swap(nextMatrix);
    }

    // If requested, compute row averages
    if (compute_average) {
        compute_row_averages(previousMatrix, n, m);
    }
		
}
