#include "setup/dividends.hpp"
#include <iostream>

std::map<int, double> parse_dividends(const std::string& dividends_str) {
    std::map<int, double> dividends_map;
    if (dividends_str.empty()) return dividends_map;
    size_t start = 0, end = 0;
    while ((end = dividends_str.find(',', start)) != std::string::npos) {
        std::string pair = dividends_str.substr(start, end - start);
        size_t colon = pair.find(':');
        if (colon != std::string::npos) {
            int step = std::stoi(pair.substr(0, colon));
            double amount = std::stod(pair.substr(colon + 1));
            dividends_map[step] = amount;
        } else if (!pair.empty()) {
            std::cerr << "Warning: Malformed dividend entry '" << pair << "'. Expected format step:amount. Skipping.\n";
        }
        start = end + 1;
    }
    std::string pair = dividends_str.substr(start);
    size_t colon = pair.find(':');
    if (colon != std::string::npos) {
        int step = std::stoi(pair.substr(0, colon));
        double amount = std::stod(pair.substr(colon + 1));
        dividends_map[step] = amount;
    } else if (!pair.empty()) {
        std::cerr << "Warning: Malformed dividend entry '" << pair << "'. Expected format step:amount. Skipping.\n";
    }
    return dividends_map;
} 