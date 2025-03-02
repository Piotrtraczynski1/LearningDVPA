#pragma once

#include <stack>
#include <variant>
#include <vector>

#include "Word.hpp"
#include "symbol/Symbols.hpp"
#include "transition/Transition.hpp"

namespace common
{
class VPA
{
    Stack stack;

public:
    VPA(transition::Transition &transition, transition::State initial)
        : delta{transition}, initialState{initial} {};

    bool checkWord(std::vector<Symbol> &word);

    VPA &operator=(const VPA &vpa);

private:
    transition::Transition &delta;
    transition::State state;
    transition::State initialState;

    void setInitialState() { state = initialState; }
};
} // namespace common