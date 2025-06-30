#pragma once
#include <vector>
#include <map>
#include <random>

class GBMSimulator {
    public:
        GBMSimulator(double S0, double r, double sigma);

        void setDividends(const std::map<int, double>& divs);
        double getRiskFreeRate() const;
        double generatePrice(double T, int steps) const;
        std::vector<double> generatePath(double T, int steps) const;
        double getVolatility() const;
        double getSpot() const;
        
    private:
        double S0_, r_, sigma_;
        std::map<int, double> dividends_;
        mutable std::mt19937 gen_;
        mutable std::normal_distribution<> norm_;
    };
