#pragma once
#include <string>

struct CLIArgs {
    double S0 = 100, K = 100, T = 1.0, r = 0.05, sigma = 0.2;
    int paths = 100000, steps = 365;
    bool verbose = false;
    std::string option_type_str = "european";
    std::string side_str = "call";
    double barrier = 0.0;
    std::string barrier_type_str;
    std::string complexity_str = "vanilla";
    double payout = 1.0;
    std::string dividends_str;
    std::string variates = "none";
};

CLIArgs parse_cli_args(int argc, char* argv[]); 