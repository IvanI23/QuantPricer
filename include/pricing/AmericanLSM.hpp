#pragma once
#include "options/american_option.hpp"
#include "simulation/GBM_Simulator.hpp"

double priceAmericanLSM(const AmericanOption& option, const GBMSimulator& simulator, int paths, int steps, bool verbose); 