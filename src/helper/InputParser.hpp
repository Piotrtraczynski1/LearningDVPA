#pragma once

#include <cstdint>
#include <tuple>

#include "helper/Command.hpp"

namespace helper
{
std::tuple<Command, int, uint16_t> parseInput(int argc, char *argv[]);
} // namespace helper