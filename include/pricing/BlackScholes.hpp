#pragma once

#include <vector>
#include <utility>
#include "options/Option.hpp"

double blackScholesPrice(double S, double K, double r, double sigma, double T, bool isCall);
double blackScholesPrice(double S0, const Option& option, double r, double sigma);
double blackScholesPriceWithDividends(double S0, double K, double r, double sigma, double T, bool isCall, double PV_divs);
double presentValueDividends(const std::vector<std::pair<double, double>>& dividends, double r);
