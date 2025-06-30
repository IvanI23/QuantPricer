#pragma once
#include <string>
#include <map>
#include "cli_args.hpp"

void print_summary(const CLIArgs& args);
void print_verbose(const CLIArgs& args, const std::map<int, double>& dividends_map);
void print_warning(const std::string& msg); 