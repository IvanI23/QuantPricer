#include "options/american_option.hpp"
#include <algorithm>

AmericanOption::AmericanOption(double strike, double maturity, OptionType type)
    : Option(strike, maturity, type) {}

double AmericanOption::payoff(double ST) const {
    return getOptionType() == OptionType::Call ? std::max(ST - getStrike(), 0.0) : std::max(getStrike() - ST, 0.0);
} 