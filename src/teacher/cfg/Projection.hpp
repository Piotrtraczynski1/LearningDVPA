#pragma once

#include <optional>

#include "teacher/cfg/NonTerminal.hpp"
#include "teacher/cfg/Terminal.hpp"

namespace teacher::cfg
{
struct Projection
{
    NonTerminal arg;
    std::optional<Terminal> terminal;
    std::optional<NonTerminal> n1;
    std::optional<NonTerminal> n2;
};

} // namespace teacher::cfg