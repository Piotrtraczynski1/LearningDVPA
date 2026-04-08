#pragma once

#include <cstdint>

#include "teacher/NonTerminal.hpp"

namespace teacher
{
enum class WitnessKind
{
    Call = 0,
    Return = 1,
    ReturnBottom = 2,
    Local = 3,
    Empty = 4
};

struct Witness
{
    WitnessKind kind;
    uint16_t symbol;
    NonTerminal firstChild;
    NonTerminal secondChild;
};
} // namespace teacher
