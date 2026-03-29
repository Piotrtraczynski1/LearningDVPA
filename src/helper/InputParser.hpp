#pragma once

#include <cstdint>
#include <string>
#include <tuple>

#include "helper/Command.hpp"

namespace helper
{
std::tuple<Command, int, uint16_t, std::string> parseInput(int argc, char *argv[]);
} // namespace helper