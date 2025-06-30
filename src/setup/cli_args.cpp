#include "setup/cli_args.hpp"
#include <cstring>

CLIArgs parse_cli_args(int argc, char* argv[]) {
    CLIArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose") args.verbose = true;
        else if (arg == "--S0" && i + 1 < argc) args.S0 = std::stod(argv[++i]);
        else if (arg == "--K" && i + 1 < argc) args.K = std::stod(argv[++i]);
        else if (arg == "--T" && i + 1 < argc) args.T = std::stod(argv[++i]);
        else if (arg == "--r" && i + 1 < argc) args.r = std::stod(argv[++i]);
        else if (arg == "--sigma" && i + 1 < argc) args.sigma = std::stod(argv[++i]);
        else if (arg == "--paths" && i + 1 < argc) args.paths = std::stoi(argv[++i]);
        else if (arg == "--steps" && i + 1 < argc) args.steps = std::stoi(argv[++i]);
        else if (arg == "--option-type" && i + 1 < argc) args.option_type_str = argv[++i];
        else if (arg == "--side" && i + 1 < argc) args.side_str = argv[++i];
        else if (arg == "--barrier" && i + 1 < argc) args.barrier = std::stod(argv[++i]);
        else if (arg == "--barrier-type" && i + 1 < argc) args.barrier_type_str = argv[++i];
        else if (arg == "--complexity" && i + 1 < argc) args.complexity_str = argv[++i];
        else if (arg == "--payout" && i + 1 < argc) args.payout = std::stod(argv[++i]);
        else if (arg == "--dividends" && i + 1 < argc) args.dividends_str = argv[++i];
        else if (arg == "--variates" && i + 1 < argc) args.variates = argv[++i];
    }
    return args;
} 