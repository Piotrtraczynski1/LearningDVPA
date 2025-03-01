#pragma once

#include <stack>
#include <variant>
#include <vector>

#include "symbol/Symbols.hpp"
#include "transition/Transition.hpp"

// tmp:
#include <iostream>

namespace common
{
class VPA
{
    using Symbol = std::variant<symbol::CallSymbol, symbol::ReturnSymbol, symbol::LocalSymbol>;

    std::stack<symbol::StackSymbol> stack; // TODO make own stack!!!!

public:
    VPA(transition::Transition &transition, transition::State initial)
        : delta{transition}, initialState{initial}
    {
        stack.push(symbol::StackSymbol::BOTTOM);
    };

    bool checkWord(std::vector<Symbol> &word)
    {
        setInitialState();
        for (auto letter : word)
        {
            switch (letter.index())
            {
            case 0:
            {
                transition::Argument<symbol::CallSymbol> callArg{
                    std::get<symbol::CallSymbol>(letter), stack.top(), state};
                transition::CoArgument coArg = delta(callArg);
                state = coArg.state;
                stack.push(coArg.stackSymbol);
                break;
            }
            case 1:
            {
                transition::Argument<symbol::ReturnSymbol> returnArg{
                    std::get<symbol::ReturnSymbol>(letter), stack.top(), state};
                state = delta(returnArg);
                if (stack.top() != symbol::StackSymbol::BOTTOM)
                    stack.pop();
                break;
            }
            case 2:
            {
                transition::Argument<symbol::LocalSymbol> localArg{
                    std::get<symbol::LocalSymbol>(letter), stack.top(), state};
                state = delta(localArg);
                break;
            }
            }
        }
        return state.isAccepted;
    }

private:
    transition::Transition &delta;
    transition::State state;
    transition::State initialState;

    void setInitialState() { state = initialState; }
};
} // namespace common