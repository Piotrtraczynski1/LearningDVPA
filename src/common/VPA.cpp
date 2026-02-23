#include "common/VPA.hpp"
#include "utils/log.hpp"

namespace common
{
VPA::VPA(
    transition::Transition &transition, transition::State initial,
    const std::vector<uint16_t> &accStates, uint16_t numStates)
    : delta{transition}, initialState{initial}, acceptingStates{}, numOfStates{numStates}
{
    sink = transition::State{numStates};
    for (const auto state : accStates)
    {
        acceptingStates[state] = true;
    }
}

bool VPA::checkWord(const Word &word)
{
    setInitialState();
    for (auto letter : word)
    {
        switch (letter.index())
        {
        case CallSymbolVariant:
        {
            transition::CoArgument coArg = delta(state, std::get<symbol::CallSymbol>(letter));
            state = coArg.state;
            stack.push(coArg.stackSymbol);
            break;
        }
        case ReturnSymbolVariant:
        {
            state = delta(state, stack.top(), std::get<symbol::ReturnSymbol>(letter));
            stack.pop();
            break;
        }
        case LocalSymbolVariant:
        {
            state = delta(state, std::get<symbol::LocalSymbol>(letter));
            break;
        }
        case ControlWordSymbolVariant:
        {
            stack = std::get<Stack>(letter);
            break;
        }
        }

        if (state == transition::State::INVALID)
        {
            state = sink;
        }
        if (stack.top() == common::symbol::StackSymbol::INVALID)
        {
            stack.pop();
            stack.push(common::symbol::StackSymbol{1}); // TODO
        }
    }

    return acceptingStates[state];
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