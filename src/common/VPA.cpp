#include "common/VPA.hpp"

namespace common
{
bool VPA::checkWord(const Word &word)
{
    setInitialState();
    for (auto letter : word)
    {
        switch (letter.index())
        {
        case 0:
        {
            transition::Argument<symbol::CallSymbol> callArg{std::get<symbol::CallSymbol>(letter),
                                                             stack.top(), state};

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
        case 3:
        {
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