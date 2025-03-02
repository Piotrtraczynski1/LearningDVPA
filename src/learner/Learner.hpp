#pragma once

#include <memory>

#include "../common/VPA.hpp"
#include "../common/Word.hpp"
#include "Selectors.hpp"
#include "TestWords.hpp"

#include "../teacher/Teacher.hpp"

namespace learner
{
class Learner
{
    teacher::Teacher oracle;
    Selectors selectors;
    TestWords testWords;
    std::shared_ptr<common::VPA> hypothesis;

    uint16_t numOfCalls;
    uint16_t numOfLocals;
    uint16_t numOfReturns;
    uint16_t numOfStackSymbols;

    std::map<common::transition::Argument<common::symbol::CallSymbol>,
             common::transition::CoArgument>
        callT;
    std::map<common::transition::Argument<common::symbol::ReturnSymbol>, common::transition::State>
        returnT;
    std::map<common::transition::Argument<common::symbol::LocalSymbol>, common::transition::State>
        localT;

    std::vector<common::transition::State> states;

public:
    Learner(teacher::Teacher teacher, uint16_t numOfC, uint16_t numOfL, uint16_t numOfR,
            uint16_t numOfS)
        : oracle{teacher}, numOfCalls{numOfC}, numOfLocals{numOfL}, numOfReturns{numOfR},
          numOfStackSymbols{numOfS} {};
    void run();

private:
    void generateAutomata();
    bool areDistinguishable(const common::Word &a, const common::Word &b);
    bool isRightCongruence(common::Word &a, common::Word &b);
    void handleCounterExample(std::shared_ptr<common::Word> counterExample);
    template <typename Symbol>
    uint16_t findEquivalentState(uint16_t stateIndex, Symbol sym, uint16_t stackIndex);

    template <typename Symbol> void handleClose(Symbol sym) {}
};
} // namespace learner