#pragma once

#include <array>
#include <variant>
#include <vector>

#include "common/Stack.hpp"
#include "common/Word.hpp"
#include "symbol/Symbols.hpp"
#include "transition/Transition.hpp"
#include "utils/Constants.hpp"

namespace teacher::cfg
{
class Converter;
}

namespace common
{
class VPA
{
    friend class teacher::Converter;

public:
    Stack stack;             // move to private
    transition::State state; // move to private
    std::array<bool, utils::MaxNumOfAutomataStates> acceptingStates;

    VPA(transition::Transition &transition, transition::State initial,
        const std::vector<uint16_t> &states, uint16_t numStates);

    bool checkWord(const Word &word);

    VPA &operator=(const VPA &vpa);

    void print(std::ostream &os = std::cout) const
    {
        os << "initial state: " << initialState << "\n";
        os << "accepting states: ";
        for (int i = 0; i < utils::MaxNumOfAutomataStates; i++)
        {
            if (acceptingStates[i])
                os << ", " << i;
        }
        os << std::endl;
        delta.print(os);
    }

    void printUt(std::ostream &os = std::cout) const
    {
        os << "numOfStates = " << numOfStates << ";\n";
        os << "acceptingStates = std::vector<uint16_t>{};\n";
        os << "numOfStackSymbols = ;\n";
        os << "numOfCalls = ;\n";
        os << "numOfReturns = ;\n";
        os << "numOfLocals = ;\n";

        os << "transition = std::make_shared<Transition>();\n\n";
        delta.printUt(os);

        os << "init()" << std::endl;
    }

private:
    void setInitialState()
    {
        state = initialState;
        stack = {};
    }

    transition::Transition &delta;
    transition::State initialState;

    uint16_t numOfStates;
};
} // namespace common