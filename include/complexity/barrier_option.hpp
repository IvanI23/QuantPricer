#pragma once
#include "options/Option.hpp"
#include <vector>
#include <algorithm>

enum class BarrierType {
    DownAndIn,
    UpAndIn,
    DownAndOut,
    UpAndOut
};

class BarrierOption : public Option {
public:
    BarrierOption(double strike, double maturity, OptionType type, double barrier, BarrierType barrierType)
        : Option(strike, maturity, type), barrier_(barrier), barrierType_(barrierType) {}

    double getBarrier() const { return barrier_; }
    BarrierType getBarrierType() const { return barrierType_; }

    double payoff(double ST) const override {
        return getOptionType() == OptionType::Call 
            ? std::max(ST - getStrike(), 0.0) 
            : std::max(getStrike() - ST, 0.0);
    }

private:
    double barrier_;
    BarrierType barrierType_;
}; 