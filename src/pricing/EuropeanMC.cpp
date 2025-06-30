#include "options/european_option.hpp"
#include "simulation/GBM_Simulator.hpp"
#include "pricing/BlackScholes.hpp"
#include <vector>
#include <cmath>
#include <iostream>

double priceEuropeanMC(const EuropeanOption& option, const GBMSimulator& simulator, int paths, bool useControlVariate, bool verbose) {
    double T = option.getMaturity();
    double K = option.getStrike();
    OptionType type = option.getOptionType();
    double r = simulator.getRiskFreeRate();
    double sigma = simulator.getVolatility();
    double S0 = simulator.getSpot();
    int sim_paths = paths;
    if (useControlVariate) {
        if (verbose) std::cout << "[Control Variate] Using Black-Scholes as control variate for European option.\n";
        double sumPayoff = 0.0;
        double sumBS = 0.0;
        std::vector<double> sample_payoffs;
        for (int i = 0; i < sim_paths; ++i) {
            double ST = simulator.generatePrice(T, 365);
            double payoff = option.payoff(ST);
            sumPayoff += payoff;
            double bs_mc_payoff = 0.0;
            if (type == OptionType::Call) bs_mc_payoff = std::max(ST - K, 0.0);
            else bs_mc_payoff = std::max(K - ST, 0.0);
            sumBS += bs_mc_payoff;
            if (verbose && i < 5) sample_payoffs.push_back(payoff);
            if (verbose && (i > 0) && (i % 10000 == 0)) {
                std::cout << "Computed " << i << " paths..." << std::endl;
            }
        }
        double mean = sumPayoff / sim_paths;
        double meanBS = sumBS / sim_paths;
        double bs_analytical = blackScholesPrice(S0, option, r, sigma);
        double cv_estimator = std::exp(-r * T) * mean + (bs_analytical - std::exp(-r * T) * meanBS);
        if (verbose) {
            std::cout << "Sample payoffs for first 5 paths: ";
            for (size_t i = 0; i < sample_payoffs.size(); ++i) {
                std::cout << sample_payoffs[i] << (i < sample_payoffs.size() - 1 ? ", " : "\n");
            }
        }
        return cv_estimator;
    }
    double sumPayoff = 0.0;
    std::vector<double> sample_payoffs;
    for (int i = 0; i < sim_paths; ++i) {
        double ST = simulator.generatePrice(T, 365);
        double payoff = option.payoff(ST);
        sumPayoff += payoff;
        if (verbose && i < 5) sample_payoffs.push_back(payoff);
        if (verbose && (i > 0) && (i % 10000 == 0)) {
            std::cout << "Computed " << i << " paths..." << std::endl;
        }
    }
    if (verbose) {
        std::cout << "Sample payoffs for first 5 paths: ";
        for (size_t i = 0; i < sample_payoffs.size(); ++i) {
            std::cout << sample_payoffs[i] << (i < sample_payoffs.size() - 1 ? ", " : "\n");
        }
    }
    double mean = sumPayoff / sim_paths;
    return std::exp(-r * T) * mean;
} 