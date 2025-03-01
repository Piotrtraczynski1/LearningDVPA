#pragma once

#include "../symbol/Symbols.hpp"
#include "State.hpp"

namespace common::transition
{
template <typename AlphabetSymbol> struct Argument
{
public:
    AlphabetSymbol alphabetSymbol;
    symbol::StackSymbol stackSymbol;
    State state;

    Argument(AlphabetSymbol as, symbol::StackSymbol ss, State s)
        : alphabetSymbol{as}, stackSymbol{ss}, state{s} {};

    bool operator<(const Argument &arg) const
    {
        if (state.identifier != arg.state.identifier)
            return state.identifier < arg.state.identifier;
        if (alphabetSymbol != arg.alphabetSymbol)
            return alphabetSymbol < arg.alphabetSymbol;
        return stackSymbol < arg.stackSymbol;
    }

    bool operator==(const Argument &arg) const
    {
        return alphabetSymbol == arg.alphabetSymbol and stackSymbol == arg.stackSymbol and
               state == arg.state;
    }
};
} // namespace common::transition