#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "learner/AutomataGenerator.hpp"
#include "learner/Selectors.hpp"
#include "learner/TestWords.hpp"
#include "teacher/Teacher.hpp"
#include "utils/log.hpp"

namespace learner
{
class Learner
{
    std::shared_ptr<common::VPA> hypothesis;

    const uint16_t numOfCalls;
    const uint16_t numOfLocals;
    const uint16_t numOfReturns;
    const uint16_t numOfStackSymbols;

    teacher::Teacher &oracle;
    std::shared_ptr<Selectors> selectors;
    std::shared_ptr<TestWords> testWords;

    AutomataGenerator generator;

public:
    Learner(
        teacher::Teacher &teacher, uint16_t numOfC, uint16_t numOfR, uint16_t numOfL,
        uint16_t numOfS);
    std::shared_ptr<common::VPA> run();

private:
    void handleCounterExample(std::shared_ptr<common::Word> counterExample);
    void
    handleStackContentDiverges(const common::Word &v, const common::Word &a, const common::Word &w);
    void
    handleSuffixesMismatch(const common::Word &v, const common::Word &a, const common::Word &w);
    void addNewSelectorIfNeeded(const common::Word &selector);

    void setInitialWord()
    {
        auto isAccepting{oracle.membershipQuery(common::Word{})};
        selectors->addSelector(common::Word{}, isAccepting);
    }
};
} // namespace learner