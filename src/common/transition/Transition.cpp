#include <iostream>

#include "common/transition/Transition.hpp"

namespace common::transition
{
template <AutomatonKind Kind>
Transition<Kind>::Transition()
{
    clear();
}

template <AutomatonKind Kind>
void Transition<Kind>::clear()
{
    std::fill(
        &callT[0][0], &callT[0][0] + Size::MaxNumOfStates * Size::MaxNumOfLetters,
        CoArgument{State::INVALID, symbol::StackSymbol::INVALID});

    std::fill(
        &returnT[0][0][0],
        &returnT[0][0][0] +
            Size::MaxNumOfStates * Size::MaxNumOfStackSymbols * Size::MaxNumOfLetters,
        State::INVALID);

    std::fill(
        &localT[0][0], &localT[0][0] + Size::MaxNumOfStates * Size::MaxNumOfLetters,
        State::INVALID);
}

template <AutomatonKind Kind>
void Transition<Kind>::add(
    State s1, symbol::CallSymbol c, State s2, symbol::StackSymbol stackSymbol)
{
    callT[s1][c] = CoArgument{s2, stackSymbol};
}

template <AutomatonKind Kind>
void Transition<Kind>::add(State s1, symbol::LocalSymbol l, State s2)
{
    localT[s1][l] = s2;
}

template <AutomatonKind Kind>
void Transition<Kind>::add(
    State s1, symbol::StackSymbol stackSymbol, symbol::ReturnSymbol r, State s2)
{
    returnT[s1][stackSymbol][r] = s2;
}

template <AutomatonKind Kind>
CoArgument Transition<Kind>::operator()(State s, symbol::CallSymbol c)
{
    return callT[s][c];
}

template <AutomatonKind Kind>
State Transition<Kind>::operator()(State s, symbol::LocalSymbol l)
{
    return localT[s][l];
}

template <AutomatonKind Kind>
State Transition<Kind>::operator()(State s, symbol::StackSymbol stackSymbol, symbol::ReturnSymbol r)
{
    return returnT[s][stackSymbol][r];
}

template <AutomatonKind Kind>
void Transition<Kind>::print(std::ostream &os) const
{
    os << "transition: [state, 'stackSymbol', alphabetSymbol] = [state, "
          "'stackSymbol']\n";
    os << "CallTransitions: " << std::endl;
    for (uint16_t state = 0; state < Size::MaxNumOfStates; state++)
    {
        for (uint16_t letter = 0; letter < Size::MaxNumOfLetters; letter++)
        {
            if (callT[state][letter].state != State::INVALID)
            {
                os << "[" << state << ", " << letter << "] = [" << callT[state][letter].state
                   << ", " << callT[state][letter].stackSymbol << "]" << std::endl;
            }
        }
    }

    os << "ReturnTransitions: " << std::endl;
    for (uint16_t state = 0; state < Size::MaxNumOfStates; state++)
    {
        for (uint16_t letter = 0; letter < Size::MaxNumOfLetters; letter++)
        {
            for (uint16_t stack = 0; stack < Size::MaxNumOfStackSymbols; stack++)
            {
                if (returnT[state][stack][letter] != State::INVALID)
                {
                    os << "[" << state << ", " << stack << ", " << letter << "] = ["
                       << returnT[state][stack][letter] << "]" << std::endl;
                }
            }
        }
    }

    os << "LocalTransitions: " << std::endl;
    for (uint16_t state = 0; state < Size::MaxNumOfStates; state++)
    {
        for (uint16_t letter = 0; letter < Size::MaxNumOfLetters; letter++)
        {
            if (localT[state][letter] != State::INVALID)
            {
                os << "[" << state << ", " << letter << "] = [" << localT[state][letter] << "]"
                   << std::endl;
            }
        }
    }
}

template <AutomatonKind Kind>
void Transition<Kind>::printUt(std::ostream &os) const
{
    os << "//CallTransitions: " << std::endl;
    for (uint16_t state = 0; state < Size::MaxNumOfStates; state++)
    {
        for (uint16_t letter = 0; letter < Size::MaxNumOfLetters; letter++)
        {
            if (callT[state][letter].state != State::INVALID)
            {
                os << "transition->add(State{" << state << "}, CS{" << letter << "}, State{"
                   << callT[state][letter].state << "}, StackSymbol{"
                   << callT[state][letter].stackSymbol << "});" << std::endl;
            }
        }
    }

    os << "\n//ReturnTransitions: " << std::endl;
    for (uint16_t state = 0; state < Size::MaxNumOfStates; state++)
    {
        for (uint16_t letter = 0; letter < Size::MaxNumOfLetters; letter++)
        {
            for (uint16_t stack = 0; stack < Size::MaxNumOfStackSymbols; stack++)
            {
                if (returnT[state][stack][letter] != State::INVALID)
                {
                    os << "transition->add(State{" << state << "}, StackSymbol{" << stack
                       << "}, RS{" << letter << "}, State{" << returnT[state][stack][letter]
                       << "});" << std::endl;
                }
            }
        }
    }

    os << "\n//LocalTransitions: " << std::endl;
    for (uint16_t state = 0; state < Size::MaxNumOfStates; state++)
    {
        for (uint16_t letter = 0; letter < Size::MaxNumOfLetters; letter++)
        {
            if (localT[state][letter] != State::INVALID)
            {
                os << "transition->add(State{" << state << "}, LS{" << letter << "}, State{"
                   << localT[state][letter] << "});" << std::endl;
            }
        }
    }
}

template class Transition<AutomatonKind::Normal>;
template class Transition<AutomatonKind::Combined>;
} // namespace common::transition
