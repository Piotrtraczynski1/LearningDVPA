#pragma once

#include <iostream>
#include <map>

#include "common/symbol/Symbols.hpp"
#include "common/transition/AutomatonSize.hpp"
#include "common/transition/CoArgument.hpp"

namespace teacher
{
class Converter;
}

namespace common::transition
{
template <AutomatonKind Kind = AutomatonKind::Normal>
class Transition
{
    friend class teacher::Converter;

    using Size = AutomatonSize<Kind>;

    CoArgument callT[Size::MaxNumOfStates][Size::MaxNumOfLetters];
    State returnT[Size::MaxNumOfStates][Size::MaxNumOfStackSymbols][Size::MaxNumOfLetters];
    State localT[Size::MaxNumOfStates][Size::MaxNumOfLetters];

public:
    Transition();
    void clear();

    Transition(const Transition &) = default;
    Transition(Transition &&) = default;
    Transition &operator=(const Transition &) = default;
    Transition &operator=(Transition &&) = default;

    void add(State s1, symbol::CallSymbol c, State s2, symbol::StackSymbol stackSymbol);
    void add(State s1, symbol::LocalSymbol l, State s2);
    void add(State s1, symbol::StackSymbol stackSymbol, symbol::ReturnSymbol r, State s2);

    CoArgument operator()(State s, symbol::CallSymbol c);
    State operator()(State s, symbol::StackSymbol stackSymbol, symbol::ReturnSymbol r);
    State operator()(State s, symbol::LocalSymbol l);

    void print(std::ostream &os = std::cout) const;
    void printUt(std::ostream &os = std::cout) const;
};
} // namespace common::transition