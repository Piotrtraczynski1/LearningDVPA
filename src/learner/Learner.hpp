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
        uint16_t numOfS)
        : oracle{teacher}, numOfCalls{numOfC}, numOfReturns{numOfR}, numOfLocals{numOfL},
          numOfStackSymbols{numOfS}, selectors{std::make_shared<Selectors>()},
          testWords{std::make_shared<TestWords>()},
          generator{oracle,       selectors,   testWords,        numOfCalls,
                    numOfReturns, numOfLocals, numOfStackSymbols}
    {
        IMP("Learner built numOfCalls: %u, numOfLocals: %u, numOfReturns: %u, numOfStackSymbols: "
            "%u",
            numOfCalls, numOfLocals, numOfReturns, numOfStackSymbols);
    };
    std::shared_ptr<common::VPA> run();

private:
    void handleCounterExample(std::shared_ptr<common::Word> counterExample);
    void
    handleStackContentDiverges(const common::Word &v, const common::Word &a, const common::Word &w);
    void
    handleSuffixesMismatch(const common::Word &v, const common::Word &a, const common::Word &w);
    void addNewSelectorIfNeeded(const common::Word &selector);
};
} // namespace learner