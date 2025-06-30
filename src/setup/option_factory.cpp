#include "setup/option_factory.hpp"
#include "options/european_option.hpp"
#include "options/american_option.hpp"
#include "complexity/barrier_option.hpp"
#include "options/asian_option.hpp"
#include "complexity/binary_option.hpp"
#include <iostream>

std::unique_ptr<Option> create_option(const CLIArgs& args, bool& is_european_vanilla) {
    OptionType side = (args.side_str == "call") ? OptionType::Call : OptionType::Put;
    is_european_vanilla = false;
    if (args.complexity_str == "barrier" && args.barrier > 0 && !args.barrier_type_str.empty()) {
        BarrierType barrierType;
        if (args.barrier_type_str == "down-and-in") barrierType = BarrierType::DownAndIn;
        else if (args.barrier_type_str == "up-and-in") barrierType = BarrierType::UpAndIn;
        else if (args.barrier_type_str == "down-and-out") barrierType = BarrierType::DownAndOut;
        else if (args.barrier_type_str == "up-and-out") barrierType = BarrierType::UpAndOut;
        else {
            std::cerr << "Error: Unknown barrier type '" << args.barrier_type_str << "'. Exiting." << std::endl;
            return nullptr;
        }
        return std::make_unique<BarrierOption>(args.K, args.T, side, args.barrier, barrierType);
    } else if (args.complexity_str == "asian") {
        return std::make_unique<AsianOption>(args.K, args.T, side);
    } else if (args.complexity_str == "binary") {
        return std::make_unique<BinaryOption>(args.K, args.T, side, args.payout);
    } else {
        if (args.option_type_str == "european") {
            is_european_vanilla = true;
            return std::make_unique<EuropeanOption>(args.K, args.T, side);
        } else if (args.option_type_str == "american") {
            return std::make_unique<AmericanOption>(args.K, args.T, side);
        } else {
            std::cerr << "Error: Unknown option type '" << args.option_type_str << "'. Defaulting to European." << std::endl;
            is_european_vanilla = true;
            return std::make_unique<EuropeanOption>(args.K, args.T, side);
        }
    }
} 