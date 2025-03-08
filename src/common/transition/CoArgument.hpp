#pragma once

#include "common/symbol/Symbols.hpp"
#include "common/transition/State.hpp"

namespace common::transition
{
struct CoArgument
{
public:
    symbol::StackSymbol stackSymbol;
    State state;

    CoArgument() = default; // Remove this constructor
    CoArgument(symbol::StackSymbol ss, State s) : stackSymbol{ss}, state{s} {};

    bool operator==(const CoArgument &coArg) const
    {
        return stackSymbol == coArg.stackSymbol and state == state;
    }
};
} // namespace common::transition