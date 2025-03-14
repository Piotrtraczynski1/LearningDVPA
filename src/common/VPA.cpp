#include "common/VPA.hpp"
#include "utils/log.hpp"

namespace common
{
VPA::VPA(
    transition::Transition &transition, transition::State initial,
    const std::vector<uint16_t> &states, uint16_t numStates)
    : delta{transition}, initialState{initial}, acceptingStates{}, numOfStates{numStates}
{
    for (const auto state : states)
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
        case 0:
        {
            transition::CoArgument coArg = delta(state, std::get<symbol::CallSymbol>(letter));
            state = coArg.state;
            stack.push(coArg.stackSymbol);
            break;
        }
        case 1:
        {
            state = delta(state, stack.top(), std::get<symbol::ReturnSymbol>(letter));
            stack.pop();
            break;
        }
        case 2:
        {
            state = delta(state, std::get<symbol::LocalSymbol>(letter));
            break;
        }
        case 3:
        {
            stack = std::get<Stack>(letter);
            break;
        }
        }

        if (state == transition::State::INVALID)
        {
            return false;
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