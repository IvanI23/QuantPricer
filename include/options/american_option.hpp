#pragma once
#include "options/Option.hpp"
#include <algorithm>
#include <stdexcept>

class AmericanOption : public Option {
public:
    AmericanOption(double strike, double maturity, OptionType type);
    double payoff(double ST) const override;
}; 