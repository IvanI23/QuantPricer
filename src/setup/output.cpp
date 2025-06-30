#include "setup/output.hpp"
#include <iostream>

void print_summary(const CLIArgs& args) {
    std::cout << "Running Monte Carlo with " << args.paths << " paths...\n";
    std::cout << "Initial Stock Price: " << args.S0 << "\n";
    std::cout << "Strike Price       : " << args.K << "\n";
    std::cout << "Volatility (σ)     : " << args.sigma << "\n";
    std::cout << "Risk-free rate (r) : " << args.r << "\n";
    std::cout << "Time to maturity   : " << args.T << " years\n";
    std::cout << "Steps per path     : " << args.steps << "\n";
    std::cout << "Option Type        : " << args.option_type_str << "\n";
    std::cout << "Option Side        : " << args.side_str << "\n";
    std::cout << "Payoff Type        : " << args.complexity_str << "\n";
}

void print_verbose(const CLIArgs& args, const std::map<int, double>& dividends_map) {
    if (!dividends_map.empty()) {
        std::cout << "Parsed dividends (step: amount):\n";
        for (const auto& kv : dividends_map) {
            std::cout << "  Step " << kv.first << ": " << kv.second << "\n";
        }
    }
}

void print_warning(const std::string& msg) {
    std::cerr << "[Warning] " << msg << std::endl;
} 