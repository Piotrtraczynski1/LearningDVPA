#pragma once

#include <cstddef>
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

inline constexpr std::size_t CallSymbolVariant = 0;
inline constexpr std::size_t ReturnSymbolVariant = 1;
inline constexpr std::size_t LocalSymbolVariant = 2;
inline constexpr std::size_t ControlWordSymbolVariant = 3;

Word operator+(const Word &a, const Word &b);
Word &operator+=(Word &lhs, const Word &rhs);
Word &operator+=(Word &lhs, const Symbol &rhs);
std::ostream &operator<<(std::ostream &os, const Symbol &symbol);
std::ostream &operator<<(std::ostream &os, const Word &word);
} // namespace common