// include/MonteCarloPricer.hpp
#pragma once

#include "options/Option.hpp"
#include "simulation/GBM_Simulator.hpp"
#include "complexity/barrier_option.hpp"
#include "options/asian_option.hpp"
#include "complexity/binary_option.hpp"
#include "options/american_option.hpp"
#include "pricing/AmericanLSM.hpp"
#include "pricing/EuropeanMC.hpp"
#include <vector>
#include <memory>

class MonteCarloPricer {
    public:
        MonteCarloPricer(int paths, bool useAntithetic = false, bool useControlVariate = false, bool verbose = false);
        double price(const Option& option, const GBMSimulator& simulator);

    private:
        double priceBarrier(const BarrierOption& option, const GBMSimulator& simulator);
        double priceAsian(const AsianOption& option, const GBMSimulator& simulator);
        double priceBinary(const BinaryOption& option, const GBMSimulator& simulator);
        int paths;
        bool useAntithetic;
        bool useControlVariate;
        bool verbose;
    };
