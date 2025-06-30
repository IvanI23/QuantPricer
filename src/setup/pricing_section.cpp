#include "setup/pricing_section.hpp"
#include "pricing/MonteCarloPricer.hpp"
#include "simulation/gbm_simulator.hpp"
#include "pricing/BlackScholes.hpp"
#include "setup/csv_exporter.hpp"
#include "options/american_option.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

void save_result_to_csv(const std::string& filename, double S0, double K, double T, double r, double sigma, int paths, double mc_price, double bs_price) {
    std::ofstream file(filename, std::ios::app);
    file << S0 << "," << K << "," << T << "," << r << "," << sigma << "," << paths << ","
         << mc_price << "," << bs_price << "," << std::abs(mc_price - bs_price) << "\n";
}

void run_pricing(const CLIArgs& args, const std::map<int, double>& dividends_map, std::unique_ptr<Option>& option, bool is_european_vanilla) {
    GBMSimulator sim(args.S0, args.r, args.sigma);
    if (!dividends_map.empty()) sim.setDividends(dividends_map);
    bool useAntithetic = (args.variates == "antithetic");
    bool useControlVariate = (args.variates == "control");
    MonteCarloPricer pricer(args.paths, useAntithetic, useControlVariate, args.verbose);
    double mc_price = pricer.price(*option, sim);
    double bs_price = 0.0;
    bool bs_valid = false;
    if (is_european_vanilla) {
        if (dividends_map.empty()) {
            bs_price = blackScholesPrice(args.S0, *option, args.r, args.sigma);
            bs_valid = true;
        } else {
            std::vector<std::pair<double, double>> divs;
            for (const auto& kv : dividends_map) {
                int step = kv.first;
                double amount = kv.second;
                double t = args.T * step / args.steps;
                divs.emplace_back(t, amount);
            }
            double PV_divs = presentValueDividends(divs, args.r);
            bs_price = blackScholesPriceWithDividends(args.S0, args.K, args.r, args.sigma, args.T, args.side_str == "call", PV_divs);
            bs_valid = true;
        }
    }
    std::cout << "--- Option Pricing ---\n";
    std::cout << "MC Price: " << mc_price << "\n";
    if (bs_valid) std::cout << "BS Price: " << bs_price << "\n";
    if (dynamic_cast<AmericanOption*>(option.get())) {
        std::cout << "[Info] American option priced using Least Squares Monte Carlo (LSM) with early exercise support.\n";
    }
    save_result_to_csv("data/results.csv", args.S0, args.K, args.T, args.r, args.sigma, args.paths, mc_price, bs_valid ? bs_price : NAN);
    GBMSimulator gbm(args.S0, args.r, args.sigma);
    if (!dividends_map.empty()) gbm.setDividends(dividends_map);
    int num_paths_to_plot = 5;
    std::vector<std::vector<double>> paths_data;
    for (int i = 0; i < num_paths_to_plot; ++i) {
        paths_data.push_back(gbm.generatePath(args.T, args.steps));
    }
    exportPathsToCSV(paths_data, "data/sample_paths.csv");
} 