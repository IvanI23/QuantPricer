#include "setup/cli_args.hpp"
#include "setup/dividends.hpp"
#include "setup/option_factory.hpp"
#include "setup/pricing_section.hpp"
#include "setup/output.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    CLIArgs args = parse_cli_args(argc, argv);
    std::map<int, double> dividends_map = parse_dividends(args.dividends_str);
    if (args.verbose) print_verbose(args, dividends_map);
    else print_summary(args);
    bool is_european_vanilla = false;
    std::unique_ptr<Option> option = create_option(args, is_european_vanilla);
    if (!option) return 1;
    run_pricing(args, dividends_map, option, is_european_vanilla);
    return 0;
} 