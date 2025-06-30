#include "setup/csv_exporter.hpp"
#include <fstream>
#include <iostream>

void exportPathsToCSV(const std::vector<std::vector<double>>& paths, const std::string& filename) {
    if (paths.empty()) {
        std::cerr << "Cannot export empty paths to CSV." << std::endl;
        return;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    for (size_t i = 0; i < paths[0].size(); ++i) {
        for (size_t j = 0; j < paths.size(); ++j) {
            file << paths[j][i];
            if (j < paths.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
} 