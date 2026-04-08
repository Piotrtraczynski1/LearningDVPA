#include <iostream>

#include "common/VPA.hpp"
#include "teacher/EquivalenceCheck.hpp"
#include "teacher/Teacher.hpp"
#include "utils/ExitCode.hpp"
#include "utils/TimeMarker.hpp"

namespace teacher
{
Teacher::Teacher(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> automaton, const uint16_t numCallsArg,
    const uint16_t numReturnsArg, const uint16_t numLocalsArg, const uint16_t stackSymbolsNumber)
    : vpa{automaton}
{
    automataCombiner = std::make_shared<teacher::AutomataCombiner<AutomatonKind::Combined>>(
        automaton, numCallsArg, numReturnsArg, numLocalsArg, stackSymbolsNumber);
    emptinessChecker = std::make_shared<teacher::EmptinessChecker>(
        numCallsArg, numReturnsArg, numLocalsArg, stackSymbolsNumber);
}

bool Teacher::membershipQuery(const common::Word &word) const
{
    TIME_MARKER("[Teacher]: membershipQuery");

    return vpa->checkWord(word);
}

common::Stack Teacher::stackContentQuery(const common::Word &word) const
{
    TIME_MARKER("[Teacher]: stackContentQuery");

    vpa->checkWord(word);
    return vpa->stack;
}

std::shared_ptr<common::Word> Teacher::equivalenceQuery(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis) const
{
    TIME_MARKER("[Teacher]: equivalenceQuery");
    LOG("[Teacher]: equivalenceQuery hypothesis numOfStates: %u", hypothesis->getNumOfStates());

    auto output = equivalenceCheck(automataCombiner, emptinessChecker, hypothesis);

    if (output->size() == 0)
    {
        return output;
    }

    for (uint16_t i = 1; i <= output->size(); i++)
    {
        common::Word testWord{(*output).begin(), (*output).begin() + i};
        if (vpa->checkWord(testWord) != hypothesis->checkWord(testWord))
        {
            return std::make_shared<common::Word>(testWord);
        }
    }

    ERR("[Teacher]: CFG output is incorrect!");
    std::cout << *output << ", vpa: " << vpa->checkWord(*output)
              << ", hypothesis: " << hypothesis->checkWord(*output) << std::endl;
    exit(toExit(ExitCode::EQUIVALENCEQUERY));
}
} // namespace teacher