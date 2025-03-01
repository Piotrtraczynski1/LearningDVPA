#pragma once

#include <variant>
#include <vector>

#include "Stack.hpp"
#include "symbol/Symbols.hpp"

namespace common
{
using ControlWord = Stack;
using Symbol =
    std::variant<symbol::CallSymbol, symbol::ReturnSymbol, symbol::LocalSymbol, ControlWord>;
using Word = std::vector<Symbol>;
} // namespace common