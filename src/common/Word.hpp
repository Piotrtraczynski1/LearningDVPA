#pragma once

#include <variant>
#include <vector>

#include "symbol/Symbols.hpp"

namespace common
{
using Symbol = std::variant<symbol::CallSymbol, symbol::ReturnSymbol, symbol::LocalSymbol>;
using Word = std::vector<Symbol>;
}