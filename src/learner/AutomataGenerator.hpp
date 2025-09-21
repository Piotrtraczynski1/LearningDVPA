#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "learner/Selectors.hpp"
#include "learner/TestWords.hpp"
#include "utils/Constants.hpp"
#include "utils/log.hpp"

#include "teacher/Teacher.hpp"

namespace learner
{
class AutomataGenerator
{
    teacher::Teacher &oracle;

    const uint16_t numOfCalls;
    const uint16_t numOfLocals;
    const uint16_t numOfReturns;
    const uint16_t numOfStackSymbols;

    std::shared_ptr<Selectors> selectors;
    std::shared_ptr<TestWords> testWords;

    common::transition::Transition transition;

    common::Word witnesses[utils::MaxNumOfAutomatonStates] = {};

public:
    AutomataGenerator(
        teacher::Teacher &oracle, std::shared_ptr<Selectors> selectors,
        std::shared_ptr<TestWords> testWords, const uint16_t calls, const uint16_t returns,
        const uint16_t locals, const uint16_t stackSymbols)
        : oracle{oracle}, selectors{selectors}, testWords{testWords}, numOfCalls{calls},
          numOfReturns{returns}, numOfLocals{locals}, numOfStackSymbols{stackSymbols}
    {
        witnesses[0] = common::Word{};
    };

    std::shared_ptr<common::VPA> generate();

    uint16_t findEquivalentSelector(const common::Word &word);
    uint16_t findEquivalentSelector(
        const uint16_t selectorIndex, const uint16_t stackIndex, const uint16_t symbolIndex);

    uint16_t findOrAddSuccessor(const common::Word &candidate);

private:
    void clearGenerator();
    void buildTransition();

    void considerCall(const uint16_t selectorIndex, const uint16_t symbolIndex);
    void considerLocal(const uint16_t selectorIndex, const uint16_t symbolIndex);
    void considerReturn(
        const uint16_t selectorIndex, const uint16_t symbolIndex, const uint16_t stackIndex);

    void
    setWitness(const uint16_t successor, const uint16_t selectorIndex, const common::Word suffix);
    template <typename T, typename K>
    void fillTransitionRegardlessOfStackSymbol(
        const T symbol, const common::transition::State state, const K coArg);

    uint16_t addSelector(const common::Word &selector);
    bool isRightCongruence(const common::Word &a, const common::Word &b) const;
    bool areDistinguishable(const common::Word &a, const common::Word &b) const;

    bool isRightCongruence(
        const common::Word &a, const uint16_t selectorIndex, const uint16_t stackIndex,
        const uint16_t symbolIndex) const;
    bool
    checkRightCongruenceWithEmptyControlLetters(const common::Word &a, const common::Word &b) const;
    bool checkRightCongruenceWithReturnLetter(
        const common::Word &a, const uint16_t selectorIndex, const uint16_t stackIndex,
        const uint16_t symbolIndex) const;
    bool isConfigurationAchievable(
        const uint16_t selectorIndex, const common::symbol::StackSymbol stackSymbol) const;
};
} // namespace learner