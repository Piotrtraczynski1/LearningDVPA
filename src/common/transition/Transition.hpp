#pragma once

#include <map>

#include "common/symbol/Symbols.hpp"
#include "common/transition/CoArgument.hpp"

#include "utils/Constants.hpp"

namespace teacher
{
class Converter;
}

namespace common::transition
{
class Transition
{
    friend class teacher::Converter;
    CoArgument callT[utils::MaxNumOfAutomataStates][utils::MaxNumOfLetters];
    State returnT[utils::MaxNumOfAutomataStates][utils::MaxNumOfStackSymbols]
                 [utils::MaxNumOfLetters];
    State localT[utils::MaxNumOfAutomataStates][utils::MaxNumOfLetters];

public:
    Transition();
    void clear();
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