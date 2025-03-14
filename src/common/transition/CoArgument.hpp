#pragma once

#include "common/symbol/Symbols.hpp"
#include "common/transition/State.hpp"

namespace common::transition
{
struct CoArgument
{
public:
    State state;
    symbol::StackSymbol stackSymbol;

    CoArgument() = default;
    CoArgument(State s, symbol::StackSymbol ss) : state{s}, stackSymbol{ss} {};

    bool operator==(const CoArgument &coArg) const
    {
        return stackSymbol == coArg.stackSymbol and state == state;
    }
};
} // namespace common::transition