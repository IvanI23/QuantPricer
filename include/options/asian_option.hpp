#pragma once
#include "options/Option.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

class AsianOption : public Option {
public:
    AsianOption(double strike, double maturity, OptionType type)
        : Option(strike, maturity, type) {}


    double payoff(const std::vector<double>& path) const {
        double avg = std::accumulate(path.begin(), path.end(), 0.0) / path.size();
        return getOptionType() == OptionType::Call
            ? std::max(avg - getStrike(), 0.0)
            : std::max(getStrike() - avg, 0.0);
    }

    double payoff(double) const override { return 0.0; }
}; 