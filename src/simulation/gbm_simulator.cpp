#include "simulation/GBM_Simulator.hpp"
#include <random>
#include <cmath>

GBMSimulator::GBMSimulator(double S0, double r, double sigma)
    : S0_(S0), r_(r), sigma_(sigma), 
    gen_(std::random_device{}()), norm_(0.0, 1.0) {}

    void GBMSimulator::setDividends(const std::map<int, double>& divs) {
        dividends_ = divs;
    }

    double GBMSimulator::getRiskFreeRate() const {
        return r_;
    }

    double GBMSimulator::getVolatility() const {
        return sigma_;
    }
    double GBMSimulator::getSpot() const {
        return S0_;
    }
    double GBMSimulator::generatePrice(double T, int steps) const {
        double dt = T / steps;
        double price = S0_;

        for (int i = 1; i <= steps; ++i) {
            double Z = norm_(gen_);
            double drift = (r_ - 0.5 * sigma_ * sigma_) * dt;
            double diffusion = sigma_ * std::sqrt(dt) * Z;
            price *= std::exp(drift + diffusion);

            auto it = dividends_.find(i);
            if (it != dividends_.end()) {
                price -= it->second;
                if (price < 0.0) price = 0.0;
            }
        }
        return price;
    }

    std::vector<double> GBMSimulator::generatePath(double T, int steps) const {
        std::vector<double> path(steps + 1);
        path[0] = S0_;
        double dt = T / steps;

        for (int i = 1; i <= steps; ++i) {
            double Z = norm_(gen_);
            double drift = (r_ - 0.5 * sigma_ * sigma_) * dt;
            double diffusion = sigma_ * std::sqrt(dt) * Z;
            path[i] = path[i-1] * std::exp(drift + diffusion);

            auto it = dividends_.find(i);
            if (it != dividends_.end()) {
                path[i] -= it->second;
                if (path[i] < 0.0) path[i] = 0.0;
            }
        }
        return path;
    }
