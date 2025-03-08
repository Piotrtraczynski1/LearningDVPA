#pragma once

#include <map>

#include "common/symbol/Symbols.hpp"
#include "common/transition/Argument.hpp"
#include "common/transition/CoArgument.hpp"

namespace common::transition
{
class Transition
{
    std::map<Argument<symbol::CallSymbol>, CoArgument> callT;
    std::map<Argument<symbol::ReturnSymbol>, State> returnT;
    std::map<Argument<symbol::LocalSymbol>, State> localT;

public:
    Transition() = default;
    Transition(std::map<Argument<symbol::CallSymbol>, CoArgument> callTransition,
               std::map<Argument<symbol::ReturnSymbol>, State> returnTransition,
               std::map<Argument<symbol::LocalSymbol>, State> localTransition)
        : callT(callTransition), returnT(returnTransition), localT(localTransition)
    {
    }

    CoArgument operator()(Argument<symbol::CallSymbol> arg);
    State operator()(Argument<symbol::ReturnSymbol> arg);
    State operator()(Argument<symbol::LocalSymbol> arg);

    void print();
};
} // namespace common::transition