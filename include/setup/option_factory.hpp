#pragma once
#include <memory>
#include "cli_args.hpp"
#include "options/Option.hpp"

std::unique_ptr<Option> create_option(const CLIArgs& args, bool& is_european_vanilla); 