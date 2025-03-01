#pragma once

#include "../symbol/Symbols.hpp"
#include "State.hpp"

namespace common::transition
{
struct CoArgument
{
public:
    symbol::StackSymbol stackSymbol;
    State state;

    CoArgument() = default; // Remove this constructor !!
    CoArgument(symbol::StackSymbol ss, State s) : stackSymbol{ss}, state{s} {};

    bool operator==(const CoArgument &coArg) const
    {
        return stackSymbol == coArg.stackSymbol and state == state;
    }
};
} // namespace common::transition