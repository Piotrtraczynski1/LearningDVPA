#pragma once

#include "utils/Constants.hpp"
#include <cstdint>

namespace teacher::cfg
{
enum class NonTerminal : uint32_t
{
    START = utils::MaxNumOfNonTerminals - 1,
    INVALID = 0xffffffff
};
} // namespace teacher::cfg