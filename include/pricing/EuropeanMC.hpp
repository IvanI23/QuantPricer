#pragma once
#include "options/european_option.hpp"
#include "simulation/GBM_Simulator.hpp"

double priceEuropeanMC(const EuropeanOption& option, const GBMSimulator& simulator, int paths, bool useControlVariate, bool verbose); 