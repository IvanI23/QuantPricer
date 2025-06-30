#pragma once
#include <map>
#include <memory>
#include "cli_args.hpp"
#include "options/Option.hpp"

void run_pricing(const CLIArgs& args, const std::map<int, double>& dividends_map, std::unique_ptr<Option>& option, bool is_european_vanilla); 