#pragma once
#include "options/Option.hpp"
#include <algorithm>

class BinaryOption : public Option {
public:
    BinaryOption(double strike, double maturity, OptionType type, double payout)
        : Option(strike, maturity, type), payout_(payout) {}

    double payoff(double ST) const override {
        if (getOptionType() == OptionType::Call)
            return (ST > getStrike()) ? payout_ : 0.0;
        else
            return (ST < getStrike()) ? payout_ : 0.0;
    }

    double getPayout() const { return payout_; }

private:
    double payout_;
}; 