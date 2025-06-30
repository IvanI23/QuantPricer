#include "pricing/MonteCarloPricer.hpp"
#include "options/Option.hpp"
#include "pricing/BlackScholes.hpp"
#include "options/asian_option.hpp"
#include "options/european_option.hpp"
#include "options/american_option.hpp"
#include "complexity/barrier_option.hpp"
#include "complexity/binary_option.hpp"
#include "pricing/AmericanLSM.hpp"
#include "pricing/EuropeanMC.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>


MonteCarloPricer::MonteCarloPricer(int paths_, bool useAntithetic_, bool useControlVariate_, bool verbose_)
    : paths(paths_), useAntithetic(useAntithetic_), useControlVariate(useControlVariate_), verbose(verbose_) {
    if (useAntithetic && useControlVariate) {
        throw std::logic_error("Antithetic variates and control variates cannot be used at the same time.");
    }
}

    double MonteCarloPricer::price(const Option& option, const GBMSimulator& simulator) {
        if (const auto* barrierOption = dynamic_cast<const BarrierOption*>(&option)) {
            return priceBarrier(*barrierOption, simulator);
        }
        if (const auto* asianOption = dynamic_cast<const AsianOption*>(&option)) {
            return priceAsian(*asianOption, simulator);
        }
        if (const auto* binaryOption = dynamic_cast<const BinaryOption*>(&option)) {
            return priceBinary(*binaryOption, simulator);
        }
        if (const auto* amOpt = dynamic_cast<const AmericanOption*>(&option)) {
            if (verbose) std::cout << "[LSM] Pricing American option with early exercise support.\n";
            return priceAmericanLSM(*amOpt, simulator, this->paths, 100, verbose);
        }
        if (const auto* euroOpt = dynamic_cast<const EuropeanOption*>(&option)) {
            return priceEuropeanMC(*euroOpt, simulator, this->paths, useControlVariate, verbose);
        }

        double T = option.getMaturity();
        double K = option.getStrike();
        OptionType type = option.getOptionType();
        double r = 0.05; 


        if (useControlVariate) {
            const auto* euroOpt = dynamic_cast<const EuropeanOption*>(&option);
            if (euroOpt) {
                if (verbose) std::cout << "[Control Variate] Using Black-Scholes as control variate for European option.\n";
                double sumPayoff = 0.0;
                double sumBS = 0.0;
                int sim_paths = useAntithetic ? paths / 2 : paths;
                std::vector<double> sample_payoffs;
                for (int i = 0; i < sim_paths; ++i) {
                    double ST = simulator.generatePrice(T, 365);
                    double payoff = euroOpt->payoff(ST);
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
                double bs_analytical = blackScholesPrice(simulator.generatePath(0,0)[0], *euroOpt, r, simulator.generatePath(0,0)[0]);
                bs_analytical = blackScholesPrice(simulator.generatePath(0,0)[0], *euroOpt, r, simulator.generatePath(0,0)[0]);
                bs_analytical = blackScholesPrice(100, *euroOpt, r, 0.2);
                double cv_estimator = std::exp(-r * T) * mean + (bs_analytical - std::exp(-r * T) * meanBS);
                if (verbose) {
                    std::cout << "Sample payoffs for first 5 paths: ";
                    for (size_t i = 0; i < sample_payoffs.size(); ++i) {
                        std::cout << sample_payoffs[i] << (i < sample_payoffs.size() - 1 ? ", " : "\n");
                    }
                }
                return cv_estimator;
            }
        }

        double sumPayoff = 0.0;
        int sim_paths = useAntithetic ? paths / 2 : paths;
        std::vector<double> sample_payoffs;

        for (int i = 0; i < sim_paths; ++i) {
            double ST = simulator.generatePrice(T, 365); 
            double payoff = option.payoff(ST);
            sumPayoff += payoff;
            if (verbose && i < 5) {
                sample_payoffs.push_back(payoff);
            }
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

    double MonteCarloPricer::priceBarrier(const BarrierOption& option, const GBMSimulator& simulator) {
        double T = option.getMaturity();
        double sumPayoff = 0.0;
        int steps = 365; 
        std::vector<double> sample_payoffs;
        for (int i = 0; i < paths; ++i) {
            std::vector<double> path = simulator.generatePath(T, steps);
            bool barrier_breached = false;
            for (const auto& price : path) {
                if ((option.getBarrierType() == BarrierType::UpAndIn || option.getBarrierType() == BarrierType::UpAndOut) && price >= option.getBarrier()) {
                    barrier_breached = true;
                    break;
                }
                if ((option.getBarrierType() == BarrierType::DownAndIn || option.getBarrierType() == BarrierType::DownAndOut) && price <= option.getBarrier()) {
                    barrier_breached = true;
                    break;
                }
            }
            bool option_is_active = false;
            switch (option.getBarrierType()) {
                case BarrierType::UpAndIn:
                case BarrierType::DownAndIn:
                    option_is_active = barrier_breached;
                    break;
                case BarrierType::UpAndOut:
                case BarrierType::DownAndOut:
                    option_is_active = !barrier_breached;
                    break;
            }
            double payoff = 0.0;
            if (option_is_active) {
                payoff = option.payoff(path.back());
                sumPayoff += payoff;
            }
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
        double mean = sumPayoff / paths;
        double r = 0.05;
        return std::exp(-r * T) * mean;
    }

    double MonteCarloPricer::priceAsian(const AsianOption& option, const GBMSimulator& simulator) {
        double T = option.getMaturity();
        double sumPayoff = 0.0;
        int steps = 365;
        std::vector<double> sample_payoffs;
        for (int i = 0; i < paths; ++i) {
            std::vector<double> path = simulator.generatePath(T, steps);
            double payoff = option.payoff(path);
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
        double mean = sumPayoff / paths;
        double r = 0.05;
        return std::exp(-r * T) * mean;
    }

    double MonteCarloPricer::priceBinary(const BinaryOption& option, const GBMSimulator& simulator) {
        double T = option.getMaturity();
        double sumPayoff = 0.0;
        std::vector<double> sample_payoffs;
        for (int i = 0; i < paths; ++i) {
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
        double mean = sumPayoff / paths;
        double r = 0.05;
        return std::exp(-r * T) * mean;
    }
