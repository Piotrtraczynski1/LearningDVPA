#include <cstdint>
#include <queue>

#include "learner/srs/ConvertedAutomata.hpp"
#include "learner/srs/SrsChecker.hpp"
#include "teacher/Converter.hpp"
#include "teacher/EquivalenceCheck.hpp"
#include "utils/log.hpp"

namespace learner::srs
{
SrsChecker::SrsChecker(
    const Srs srsArg, const teacher::Teacher &oracleArg, const uint16_t numOfCallsArg,
    const uint16_t numOfReturnArg, const uint16_t numOfLocalsArg,
    const uint16_t numOfStackSymbolsArg)
    : srs{srsArg}, oracle{oracleArg}, converter{common::symbol::LocalSymbol{numOfLocalsArg}},
      numOfCalls{numOfCallsArg}, numOfReturns{numOfReturnArg}, numOfLocals{numOfLocalsArg},
      numOfStackSymbols{numOfStackSymbolsArg},
      specialSymbol{common::symbol::LocalSymbol{numOfLocalsArg}}
{
}

common::Word SrsChecker::check(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    prepareWellMatchedWords(hypothesis);

    for (const auto &word : wellMatchedWords)
    {
        for (const auto &rule : srs)
        {
            SrsRule srsRule{.l = word, .r = rule.l + word + rule.r};
            ConvertedAutomata convertedAutomata{converter.run(hypothesis, srsRule)};
            auto word{checkEquivalence(convertedAutomata)};
            if (word != emptyWord)
            {
                return prepareCounterexample(hypothesis, word, srsRule);
            }
        }
    }

    return common::Word{};
}

void SrsChecker::prepareWellMatchedWords(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    wellMatchedWords.clear();

    // TODO
    for (uint16_t localId = 0; localId < numOfLocals; localId++)
    {
        wellMatchedWords.push_back(common::Word{common::symbol::LocalSymbol{localId}});
    }
}

common::Word SrsChecker::checkEquivalence(const ConvertedAutomata &convertedAutomata)
{
    auto converter = std::make_shared<teacher::Converter>(
        convertedAutomata.lAutomaton, numOfCalls, numOfReturns, numOfLocals + 1, numOfStackSymbols);
    auto output = teacher::equivalenceCheck(converter, convertedAutomata.rAutomaton);

    common::Symbol leftoverSymbol{common::symbol::ReturnSymbol{numOfReturns}};
    for (uint16_t i = 0; i < output->size(); i++)
    {
        if ((*output)[i] == leftoverSymbol)
        {
            return common::Word{(*output).begin(), (*output).begin() + i};
        }
    }

    return *output;
}

common::Word SrsChecker::prepareCounterexample(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const common::Word &word,
    const SrsRule &srsRule)
{
    common::Word firstCandidate{};
    common::Word secondCandidate{};

    for (const auto &symbol : word)
    {
        if (symbol == specialSymbol)
        {
            firstCandidate += srsRule.l;
            secondCandidate += srsRule.r;
        }
        else
        {
            firstCandidate += symbol;
            secondCandidate += symbol;
        }
    }

    if (oracle.membershipQuery(firstCandidate) != hypothesis->checkWord(firstCandidate))
    {
        return firstCandidate;
    }

    return secondCandidate;
}
} // namespace learner::srs