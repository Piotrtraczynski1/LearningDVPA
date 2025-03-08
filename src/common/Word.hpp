#pragma once

#include <iostream>
#include <variant>
#include <vector>

#include "common/Stack.hpp"
#include "common/symbol/Symbols.hpp"

namespace common
{
using ControlWord = Stack;
using Symbol =
    std::variant<symbol::CallSymbol, symbol::ReturnSymbol, symbol::LocalSymbol, ControlWord>;
using Word = std::vector<Symbol>;

Word operator+(const Word &a, const Word &b);
Word &operator+=(Word &lhs, const Word &rhs);
Word &operator+=(Word &lhs, const Symbol &rhs);
std::ostream &operator<<(std::ostream &os, const Symbol &symbol);
std::ostream &operator<<(std::ostream &os, const Word &word);
} // namespace common