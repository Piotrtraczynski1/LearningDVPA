#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "learner/srs/AutomataConverter.hpp"
#include "learner/srs/Srs.hpp"
#include "teacher/Teacher.hpp"

namespace learner::srs
{
class SrsChecker
{
    const Srs srs;
    const teacher::Teacher &oracle;
    AutomataConverter converter;

    const uint16_t numOfCalls;
    const uint16_t numOfReturns;
    const uint16_t numOfLocals;
    const uint16_t numOfStackSymbols;
    common::Symbol specialSymbol;

    std::vector<common::Word> wellMatchedWords{};

    const common::Word emptyWord{};

public:
    SrsChecker(
        const Srs srsArg, const teacher::Teacher &oracleArg, const uint16_t numOfCallsArg,
        const uint16_t numOfReturnArg, const uint16_t numOfLocalsArg,
        const uint16_t numOfStackSymbolsArg);

    common::Word check(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis);

private:
    common::Word checkConfigurationsConsistency(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis,
        const common::Word &lhs, const common::Word &rhs) const;

    void prepareWellMatchedWords(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis);

    common::Word checkEquivalence(const ConvertedAutomata &convertedAutomata);
    common::Word prepareCounterexample(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis,
        const common::Word &word, const SrsRule &srsRule);
};
} // namespace learner::srs
