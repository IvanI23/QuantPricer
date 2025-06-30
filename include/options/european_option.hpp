#pragma once
#include "options/Option.hpp"
#include <algorithm>

class EuropeanOption : public Option {
public:
    EuropeanOption(double strike, double maturity, OptionType type);
    double payoff(double ST) const override;
};