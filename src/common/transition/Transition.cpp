#include <iostream>

#include "common/transition/Transition.hpp"

namespace common::transition
{
CoArgument Transition::operator()(Argument<symbol::CallSymbol> arg)
{
    return callT.at(arg);
}

State Transition::operator()(Argument<symbol::LocalSymbol> arg)
{
    return localT.at(arg);
}

State Transition::operator()(Argument<symbol::ReturnSymbol> arg)
{
    return returnT.at(arg);
}

void Transition::print()
{
    std::cout << "transition: [alphabetSymbol, stackSymbol, state.identifier] = [state.identifier, "
                 "'stackSymbol']\n";
    std::cout << "CallTranstions: " << callT.size() << std::endl;
    for (const auto &call : callT)
    {
        std::cout << "[" << call.first.alphabetSymbol << ", " << call.first.stackSymbol << ", "
                  << call.first.state.identifier << "] = [" << call.second.state.identifier << ", "
                  << call.second.stackSymbol << "]" << std::endl;
    }

    std::cout << "ReturnTransitions: \n";
    for (const auto &ret : returnT)
    {
        std::cout << "[" << ret.first.alphabetSymbol << ", " << ret.first.stackSymbol << ", "
                  << ret.first.state.identifier << "] = [" << ret.second.identifier << "]"
                  << std::endl;
    }

    std::cout << "LocalTransitions: \n";
    for (const auto &loc : localT)
    {
        std::cout << "[" << loc.first.alphabetSymbol << ", " << loc.first.stackSymbol << ", "
                  << loc.first.state.identifier << "] = [" << loc.second.identifier << "]"
                  << std::endl;
    }
}
} // namespace common::transition
