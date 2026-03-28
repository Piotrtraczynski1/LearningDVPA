#include "common/VPA.hpp"
#include "utils/log.hpp"

namespace common
{
template <AutomatonKind Kind>
VPA<Kind>::VPA(
    std::unique_ptr<transition::Transition<Kind>> transition, transition::State initial,
    const std::vector<uint16_t> &accStates, uint16_t numStates)
    : delta{std::move(transition)}, initialState{initial}, acceptingStates{}, numOfStates{numStates}
{
    sink = transition::State{numStates};
    for (const auto state : accStates)
    {
        acceptingStates[state] = true;
    }
}

template <AutomatonKind Kind>
VPA<Kind>::VPA(const VPA<Kind> &vpa)
    : stack{vpa.stack}, state{vpa.state}, acceptingStates{vpa.acceptingStates},
      delta{std::make_unique<transition::Transition<Kind>>(*vpa.delta)},
      initialState{vpa.initialState}, sink{vpa.sink}, numOfStates{vpa.numOfStates}
{
}

template <AutomatonKind Kind>
VPA<Kind> &VPA<Kind>::operator=(const VPA<Kind> &vpa)
{
    if (this == &vpa)
        return *this;

    delta = std::make_unique<transition::Transition<Kind>>(*vpa.delta);
    state = vpa.state;
    initialState = vpa.initialState;
    stack = vpa.stack;
    acceptingStates = vpa.acceptingStates;
    sink = vpa.sink;
    numOfStates = vpa.numOfStates;

    return *this;
}

template <AutomatonKind Kind>
bool VPA<Kind>::checkWord(const Word &word)
{
    setInitialState();
    readWord(word);

    return acceptingStates[state];
}

template <AutomatonKind Kind>
transition::State VPA<Kind>::getSink() const
{
    return sink;
}

template <AutomatonKind Kind>
uint16_t VPA<Kind>::getNumOfStates() const
{
    return numOfStates;
}

template <AutomatonKind Kind>
void VPA<Kind>::print(std::ostream &os) const
{
    os << "initial state: " << initialState << "\n";
    os << "accepting states: ";
    for (int i = 0; i < Size::MaxNumOfStates; i++)
    {
        if (acceptingStates[i])
            os << ", " << i;
    }
    os << std::endl;
    delta->print(os);
}

template <AutomatonKind Kind>
void VPA<Kind>::printUt(std::ostream &os) const
{
    os << "numOfStates = " << numOfStates << ";\n";
    os << "acceptingStates = std::vector<uint16_t>{";
    for (int i = 0; i < Size::MaxNumOfStates; i++)
    {
        if (acceptingStates[i])
            os << ", " << i;
    }
    os << "};\n";
    os << "numOfStackSymbols = ;\n";
    os << "numOfCalls = ;\n";
    os << "numOfReturns = ;\n";
    os << "numOfLocals = ;\n";

    os << "transition = std::make_unique<Transition<AutomatonKind::Normal>>();\n\n";
    delta->printUt(os);

    os << "init();" << std::endl;
}

template <AutomatonKind Kind>
void VPA<Kind>::setInitialState()
{
    state = initialState;
    stack = {};
}

template <AutomatonKind Kind>
void VPA<Kind>::readWord(const Word &word)
{
    for (auto letter : word)
    {
        switch (letter.index())
        {
        case CallSymbolVariant:
        {
            transition::CoArgument coArg = (*delta)(state, std::get<symbol::CallSymbol>(letter));
            state = coArg.state;
            stack.push(coArg.stackSymbol);
            break;
        }
        case ReturnSymbolVariant:
        {
            state = (*delta)(state, stack.top(), std::get<symbol::ReturnSymbol>(letter));
            stack.pop();
            break;
        }
        case LocalSymbolVariant:
        {
            state = (*delta)(state, std::get<symbol::LocalSymbol>(letter));
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
}

template class VPA<AutomatonKind::Normal>;
template class VPA<AutomatonKind::Combined>;
} // namespace common