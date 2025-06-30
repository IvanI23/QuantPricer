#include "options/Option.hpp"
#include <algorithm>

double Option::payoff(double stockPrice) const {
    if (optType == OptionType::Call) {
        return std::max(stockPrice - K, 0.0);
    } else {
        return std::max(K - stockPrice, 0.0);
    }
}
