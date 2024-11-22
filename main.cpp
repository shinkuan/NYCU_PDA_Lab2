#include <iostream>
#include <chrono>
#include <vector>
#include <omp.h>
#include "fast_sp.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <alpha> <block_file> <net_file> <output_file>" << std::endl;
        return 1;
    }

    double alpha = std::stod(argv[1]);
    std::string block_file = argv[2];
    std::string net_file = argv[3];
    std::string output_file = argv[4];

    int num_runs = omp_get_max_threads();

    std::vector<double> costs(num_runs);
    std::vector<FastSP*> fastsp_instances(num_runs);

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for
    for (int i = 0; i < num_runs; ++i) {
        FastSP* fastsp = new FastSP(alpha, block_file, net_file);
        fastsp->initialize();
        fastsp_instances[i] = fastsp;
    }

    for (int i = 0; i < 10; ++i) {
        #pragma omp parallel for
        for (int j = 0; j < num_runs; ++j) {
            double cost = fastsp_instances[j]->runFastSP(28.5);
            costs[j] = cost;
        }
        // Find the best solution
        // Copy the best solution to all instances
        // Run the instances again
        double min_cost = costs[0];
        int min_index = 0;
        for (int j = 1; j < num_runs; ++j) {
            if (costs[j] < min_cost) {
                min_cost = costs[j];
                min_index = j;
            }
        }

        std::cout << "Min Cost: " << min_cost << std::endl;

        for (int j = 0; j < num_runs; ++j) {
            if (j != min_index) {
                fastsp_instances[j]->copySolution(fastsp_instances[min_index]);
            }
        }
    }


    double min_cost = costs[0];
    int min_index = 0;
    for (int i = 1; i < num_runs; ++i) {
        if (costs[i] < min_cost) {
            min_cost = costs[i];
            min_index = i;
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    double runtime = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count();

    fastsp_instances[min_index]->outputDump(output_file, runtime);
    std::cout << "Elapsed: " << runtime << " Sec" << std::endl;

    for (int i = 0; i < num_runs; ++i) {
        delete fastsp_instances[i];
    }

    return 0;
}