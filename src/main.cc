// main.cpp
#include "cpu_heat.h"
#include <iostream>
#include <string>
#include <cstdlib> // for atoi

int main(

int n = 32; // default rows
    int m = 32; // default columns
    int p = 10; // default iterations
    bool compute_average = false;

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
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    // Run the CPU simulation
    run_cpu_simulation(n, m, p, compute_average);
		
    return 0;
}

