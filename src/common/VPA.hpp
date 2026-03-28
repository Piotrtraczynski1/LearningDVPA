#pragma once

#include <array>
#include <variant>
#include <vector>

#include "common/Stack.hpp"
#include "common/Word.hpp"
#include "common/transition/AutomatonSize.hpp"
#include "symbol/Symbols.hpp"
#include "transition/Transition.hpp"

namespace teacher::cfg
{
class Converter;
}

namespace common
{
template <AutomatonKind Kind = AutomatonKind::Normal>
class VPA
{
    friend class teacher::Converter;
    using Size = AutomatonSize<Kind>;

public:
    Stack stack;
    transition::State state;
    std::array<bool, Size::MaxNumOfStates> acceptingStates;

    VPA(transition::Transition<Kind> &transition, transition::State initial,
        const std::vector<uint16_t> &accStates, uint16_t numStates);

    bool checkWord(const Word &word);

    VPA &operator=(const VPA &vpa);

    transition::State getSink() const
    {
        return sink;
    }

    uint16_t getNumOfStates() const
    {
        return numOfStates;
    }

    void print(std::ostream &os = std::cout) const
    {
        os << "initial state: " << initialState << "\n";
        os << "accepting states: ";
        for (int i = 0; i < Size::MaxNumOfStates; i++)
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

        os << "transition = std::make_shared<Transition>();\n\n";
        delta.printUt(os);

        os << "init();" << std::endl;
    }

private:
    void setInitialState()
    {
        state = initialState;
        stack = {};
    }

    transition::Transition<Kind> &delta;
    transition::State initialState;
    transition::State sink;

    uint16_t numOfStates;
};
} // namespace common