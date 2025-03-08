#pragma once

#include <stack>
#include <variant>
#include <vector>

#include "common/Word.hpp"
#include "symbol/Symbols.hpp"
#include "transition/Transition.hpp"

namespace common
{
class VPA
{
public:
    Stack stack; // move to private

    VPA(transition::Transition &transition, transition::State initial)
        : delta{transition}, initialState{initial} {};

    bool checkWord(const Word &word);

    VPA &operator=(const VPA &vpa);

    void print() const
    {
        delta.print();
    }

private:
    void setInitialState()
    {
        state = initialState;
    }

    transition::Transition &delta;
    transition::State state;
    transition::State initialState;
};
} // namespace common