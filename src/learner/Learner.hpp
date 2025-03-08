#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "learner/Selectors.hpp"
#include "learner/TestWords.hpp"

#include "teacher/Teacher.hpp"

namespace learner
{
class Learner
{
    teacher::Teacher oracle;
    Selectors selectors;
    TestWords testWords;
    std::shared_ptr<common::VPA> hypothesis;

    const uint16_t numOfCalls;
    const uint16_t numOfLocals;
    const uint16_t numOfReturns;
    const uint16_t numOfStackSymbols;

    std::map<
        common::transition::Argument<common::symbol::CallSymbol>, common::transition::CoArgument>
        callT;
    std::map<common::transition::Argument<common::symbol::ReturnSymbol>, common::transition::State>
        returnT;
    std::map<common::transition::Argument<common::symbol::LocalSymbol>, common::transition::State>
        localT;

    std::vector<common::transition::State> states;

public:
    Learner(
        teacher::Teacher teacher, uint16_t numOfC, uint16_t numOfR, uint16_t numOfL,
        uint16_t numOfS)
        : oracle{teacher}, numOfCalls{numOfC}, numOfReturns{numOfR}, numOfLocals{numOfL},
          numOfStackSymbols{numOfS} {};
    std::shared_ptr<common::VPA> run();

private:
    void generateAutomata();
    bool areDistinguishable(const common::Word &a, const common::Word &b) const;
    bool isRightCongruence(const common::Word &a, const common::Word &b) const;
    void handleCounterExample(std::shared_ptr<common::Word> counterExample);
    uint16_t findEquivalentState(const common::Word word);
    bool isAppropriateSplit(const common::Word &word) const;
};
} // namespace learner