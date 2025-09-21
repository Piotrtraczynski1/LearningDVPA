#pragma once

#include "utils/Constants.hpp"
#include <cstdint>

namespace common::symbol
{
enum StackSymbol : uint16_t
{
    BOTTOM = 0,
    INVALID = utils::MaxNumOfCombinedStackSymbols - 1
};
} // namespace common::symbol