#pragma once

#include <set>

#include "common/symbol/Symbols.hpp"
#include "common/transition/State.hpp"

namespace learner
{
struct UndefinedTransition
{
    common::transition::State state;
    common::symbol::StackSymbol stackSymbol;
    common::symbol::ReturnSymbol returnSymbol;

    bool operator<(UndefinedTransition const &other) const
    {
        if (state != other.state)
            return state < other.state;

        if (stackSymbol != other.stackSymbol)
            return stackSymbol < other.stackSymbol;

        return returnSymbol < other.returnSymbol;
    }
};

using UndefinedTransitions = std::set<UndefinedTransition>;
} // namespace learner