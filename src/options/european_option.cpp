#include "options/european_option.hpp"
#include <algorithm>

EuropeanOption::EuropeanOption(double strike, double maturity, OptionType type)
    : Option(strike, maturity, type) {}

double EuropeanOption::payoff(double ST) const {
    return getOptionType() == OptionType::Call ? std::max(ST - getStrike(), 0.0) : std::max(getStrike() - ST, 0.0);
} 