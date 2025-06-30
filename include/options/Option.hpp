// include/Option.hpp
#ifndef OPTION_HPP
#define OPTION_HPP

enum class OptionType { Call, Put };

class Option {
public:
    Option(double strike, double maturity, OptionType type)
        : K(strike), T(maturity), optType(type) {}
    virtual ~Option() = default;

    double getStrike() const { return K; }
    double getMaturity() const { return T; }
    OptionType getOptionType() const { return optType; }

    virtual double payoff(double stockPrice) const = 0;

private:
    double K;
    double T;
    OptionType optType;
};

#endif
