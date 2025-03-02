#include "VPA.hpp"

// tmp:
#include <iostream>

namespace common
{ // TODO: Refactor
bool VPA::checkWord(std::vector<Symbol> &word)
{
    setInitialState();
    for (auto letter : word)
    {
        switch (letter.index())
        {
        case 0:
        {
            std::cout << "call: " << std::get<symbol::CallSymbol>(letter) << ", " << stack.top()
                      << ", " << state.identifier << std::endl;
            transition::Argument<symbol::CallSymbol> callArg{std::get<symbol::CallSymbol>(letter),
                                                             stack.top(), state};
            transition::CoArgument coArg = delta(callArg);
            state = coArg.state;
            stack.push(coArg.stackSymbol);
            break;
        }
        case 1:
        {
            std::cout << "return: " << std::get<symbol::ReturnSymbol>(letter) << ", " << stack.top()
                      << ", " << state.identifier << std::endl;
            transition::Argument<symbol::ReturnSymbol> returnArg{
                std::get<symbol::ReturnSymbol>(letter), stack.top(), state};
            state = delta(returnArg);
            if (stack.top() != symbol::StackSymbol::BOTTOM)
                stack.pop();
            break;
        }
        case 2:
        {
            std::cout << "local" << std::endl;
            transition::Argument<symbol::LocalSymbol> localArg{
                std::get<symbol::LocalSymbol>(letter), stack.top(), state};
            state = delta(localArg);
            break;
        }
        case 3:
        {
            std::cout << "control" << std::endl;
            stack = std::get<Stack>(letter);
            break;
        }
        }
    }
    return state.isAccepted;
}

VPA &VPA::operator=(const VPA &vpa)
{
    delta = vpa.delta;
    state = vpa.state;
    initialState = vpa.initialState;
    stack = vpa.stack;
    return *this;
}
} // namespace common