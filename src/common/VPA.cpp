#include "common/VPA.hpp"
#include "utils/log.hpp"

namespace common
{
template <AutomatonKind Kind>
VPA<Kind>::VPA(
    transition::Transition<Kind> &transition, transition::State initial,
    const std::vector<uint16_t> &accStates, uint16_t numStates)
    : delta{transition}, initialState{initial}, acceptingStates{}, numOfStates{numStates}
{
    sink = transition::State{numStates};
    for (const auto state : accStates)
    {
        acceptingStates[state] = true;
    }
}

template <AutomatonKind Kind>
bool VPA<Kind>::checkWord(const Word &word)
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

template <AutomatonKind Kind>
VPA<Kind> &VPA<Kind>::operator=(const VPA<Kind> &vpa)
{
    delta = vpa.delta;
    state = vpa.state;
    initialState = vpa.initialState;
    stack = vpa.stack;
    return *this;
}

template class VPA<AutomatonKind::Normal>;
template class VPA<AutomatonKind::Combined>;
} // namespace common