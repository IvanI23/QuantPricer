#include "options/american_option.hpp"
#include "simulation/GBM_Simulator.hpp"
#include <vector>
#include <cmath>
#include <iostream>


double priceAmericanLSM(const AmericanOption& option, const GBMSimulator& simulator, int paths, int steps, bool verbose) {
    double T = option.getMaturity();
    double dt = T / steps;
    if (verbose) {
        std::cout << "[LSM] Generating " << paths << " paths with " << steps << " time steps..." << std::endl;
    }
    std::vector<std::vector<double>> all_paths(paths, std::vector<double>(steps + 1));
    for (int i = 0; i < paths; ++i) {
        all_paths[i] = simulator.generatePath(T, steps);
        if (verbose && (i > 0) && (i % 10000 == 0)) {
            std::cout << "[LSM] Generated " << i << " paths..." << std::endl;
        }
    }
    if (verbose) {
        std::cout << "[LSM] Computing cashflows for all paths..." << std::endl;
    }
    std::vector<std::vector<double>> cashflows(paths, std::vector<double>(steps + 1, 0.0));
    for (int i = 0; i < paths; ++i) {
        for (int t = 0; t <= steps; ++t) {
            cashflows[i][t] = option.payoff(all_paths[i][t]);
        }
        if (verbose && (i > 0) && (i % 10000 == 0)) {
            std::cout << "[LSM] Computed cashflows for " << i << " paths..." << std::endl;
        }
    }
    std::vector<double> discounts(steps + 1, 1.0);
    double r = simulator.getRiskFreeRate();
    for (int t = 1; t <= steps; ++t) {
        discounts[t] = std::exp(-r * dt * t);
    }
    if (verbose) {
        std::cout << "[LSM] Starting backward induction with LSM regression..." << std::endl;
    }
    std::vector<double> values(paths, 0.0);
    std::vector<int> exercise(paths, steps);
    for (int t = steps - 1; t >= 1; --t) {
        if (verbose && (t % 20 == 0 || t == steps - 1)) {
            std::cout << "[LSM] Processing time step " << t << "/" << steps << "..." << std::endl;
        }
        std::vector<int> itm_indices;
        std::vector<double> X, Y;
        for (int i = 0; i < paths; ++i) {
            if (cashflows[i][t] > 1e-8) {
                itm_indices.push_back(i);
                X.push_back(all_paths[i][t]);
                Y.push_back(values[i] * std::exp(-r * dt));
            }
        }
        if (!X.empty()) {
            double Sx = 0, Sx2 = 0, Sx3 = 0, Sx4 = 0, Sy = 0, Sxy = 0, Sx2y = 0;
            int N = X.size();
            for (int i = 0; i < N; ++i) {
                double x = X[i], y = Y[i];
                double x2 = x * x;
                Sx += x;
                Sx2 += x2;
                Sx3 += x2 * x;
                Sx4 += x2 * x2;
                Sy += y;
                Sxy += x * y;
                Sx2y += x2 * y;
            }
            double det = N * (Sx2 * Sx4 - Sx3 * Sx3)
                        - Sx * (Sx * Sx4 - Sx2 * Sx3)
                        + Sx2 * (Sx * Sx3 - Sx2 * Sx2);
            double a = 0, b = 0, c = 0;
            if (std::abs(det) > 1e-12) {
                a = (Sy * (Sx2 * Sx4 - Sx3 * Sx3)
                   - Sx * (Sxy * Sx4 - Sx3 * Sx2y)
                   + Sx2 * (Sxy * Sx3 - Sx2 * Sx2y)) / det;
                b = (N * (Sxy * Sx4 - Sx3 * Sx2y)
                   - Sy * (Sx * Sx4 - Sx2 * Sx3)
                   + Sx2 * (Sx * Sx2y - Sxy * Sx2)) / det;
                c = (N * (Sx2 * Sx2y - Sxy * Sx3)
                   - Sx * (Sx * Sx2y - Sxy * Sx2)
                   + Sy * (Sx * Sx3 - Sx2 * Sx2)) / det;
            }
            int exercise_count = 0;
            for (size_t idx = 0; idx < itm_indices.size(); ++idx) {
                int i = itm_indices[idx];
                double continuation = a + b * X[idx] + c * X[idx] * X[idx];
                if (cashflows[i][t] > continuation) {
                    values[i] = cashflows[i][t];
                    exercise[i] = t;
                    exercise_count++;
                } else {
                    values[i] = values[i] * std::exp(-r * dt);
                }
            }
            if (verbose && (t % 20 == 0 || t == steps - 1)) {
                std::cout << "[LSM] Step " << t << ": " << exercise_count << " early exercises out of " << itm_indices.size() << " ITM paths" << std::endl;
            }
        } else {
            for (int i = 0; i < paths; ++i) {
                values[i] = values[i] * std::exp(-r * dt);
            }
        }
    }
    double sum = 0.0;
    for (int i = 0; i < paths; ++i) {
        sum += values[i] * discounts[exercise[i]];
    }
    double price = sum / paths;
    if (verbose) {
        std::cout << "[LSM] American option pricing completed." << std::endl;
        std::cout << "[LSM] Final price: " << price << std::endl;
    }
    return price;
} 