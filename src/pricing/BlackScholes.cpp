#include "pricing/BlackScholes.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double normCDF(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double blackScholesPrice(double S0, const Option& option, double r, double sigma) {
    double K = option.getStrike();
    double T = option.getMaturity();
    OptionType type = option.getOptionType();

    double d1 = (std::log(S0 / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    if (type == OptionType::Call)
        return S0 * normCDF(d1) - K * std::exp(-r * T) * normCDF(d2);
    else
        return K * std::exp(-r * T) * normCDF(-d2) - S0 * normCDF(-d1);
}

double blackScholesPrice(double S, double K, double r, double sigma, double T, bool isCall) {
    double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    if (isCall) {
        return S * normCDF(d1) - K * std::exp(-r * T) * normCDF(d2);
    } else {
        return K * std::exp(-r * T) * normCDF(-d2) - S * normCDF(-d1);
    }
}

double blackScholesPriceWithDividends(double S0, double K, double r, double sigma, double T, bool isCall, double PV_divs) {
    double S0_adj = S0 - PV_divs;
    return blackScholesPrice(S0_adj, K, r, sigma, T, isCall);
}

double presentValueDividends(const std::vector<std::pair<double, double>>& dividends, double r) {
    double pv = 0.0;
    for (const auto& div : dividends) {
        double t = div.first;
        double amount = div.second;
        pv += amount * std::exp(-r * t);
    }
    return pv;
}
