#include <cstring>

#include "learner/AutomataGenerator.hpp"
#include "utils/TimeMarker.hpp"

namespace learner
{
std::shared_ptr<common::VPA> AutomataGenerator::generate()
{
    TIME_MARKER("generateAutomata");
    LOG("[AutomataGenerator]: Generating Automata");

    clearGenerator();
    buildTransition();

    return std::make_shared<common::VPA>(common::VPA{
        transition, selectors->getState(0), selectors->getAcceptingStates(), selectors->size()});
}

void AutomataGenerator::clearGenerator()
{
    transition.clear();
}

void AutomataGenerator::buildTransition()
{
    for (uint16_t selectorIndex = 0; selectorIndex < selectors->size(); selectorIndex++)
    {
        for (uint16_t symbolIndex = 0; symbolIndex < numOfCalls; symbolIndex++)
        {
            considerCall(selectorIndex, symbolIndex);
        }

        for (uint16_t symbolIndex = 0; symbolIndex < numOfLocals; symbolIndex++)
        {
            considerLocal(selectorIndex, symbolIndex);
        }

        for (uint16_t symbolIndex = 0; symbolIndex < numOfReturns; symbolIndex++)
        {
            for (uint16_t stackIndex = 0; stackIndex < numOfStackSymbols; stackIndex++)
            {
                considerReturn(selectorIndex, symbolIndex, stackIndex);
            }
        }
    }
}

void AutomataGenerator::considerCall(const uint16_t selectorIndex, const uint16_t symbolIndex)
{
    common::Word candidate{(*selectors)[selectorIndex]};
    candidate += common::Word{common::symbol::CallSymbol{symbolIndex}};

    uint16_t successor{findOrAddSuccessor(candidate)};

    common::symbol::StackSymbol pushedStackSymbol{oracle.stackContentQuery(candidate).top()};

    transition.add(
        selectors->getState(selectorIndex), common::symbol::CallSymbol{symbolIndex},
        selectors->getState(successor), pushedStackSymbol);

    setWitness(successor, selectorIndex, common::Word{common::symbol::CallSymbol{symbolIndex}});
}

void AutomataGenerator::considerLocal(const uint16_t selectorIndex, const uint16_t symbolIndex)
{
    common::Word candidate{(*selectors)[selectorIndex]};
    candidate += common::Word{common::symbol::LocalSymbol{symbolIndex}};

    uint16_t successor{findOrAddSuccessor(candidate)};

    transition.add(
        selectors->getState(selectorIndex), common::symbol::LocalSymbol{symbolIndex},
        selectors->getState(successor));

    setWitness(successor, selectorIndex, common::Word{common::symbol::LocalSymbol{symbolIndex}});
}

void AutomataGenerator::considerReturn(
    const uint16_t selectorIndex, const uint16_t symbolIndex, const uint16_t stackIndex)
{
    common::Word candidate{(*selectors)[selectorIndex]};
    candidate += common::Word{common::symbol::ReturnSymbol{symbolIndex}};

    uint16_t successor{findEquivalentSelector(selectorIndex, stackIndex, symbolIndex)};

    if (successor == Selectors::INVALID_INDEX)
    {
        if (not isConfigurationAchievable(selectorIndex, common::symbol::StackSymbol{stackIndex}))
        {
            return;
        }

        successor = addSelector(candidate);
    }

    transition.add(
        selectors->getState(selectorIndex), common::symbol::StackSymbol{stackIndex},
        common::symbol::ReturnSymbol{symbolIndex}, selectors->getState(successor));

    setWitness(successor, selectorIndex, common::Word{common::symbol::ReturnSymbol{symbolIndex}});
}

uint16_t AutomataGenerator::findOrAddSuccessor(const common::Word &candidate)
{
    uint16_t successor{findEquivalentSelector(candidate)};

    if (successor == Selectors::INVALID_INDEX)
    {
        successor = addSelector(candidate);
    }

    return successor;
}

void AutomataGenerator::setWitness(
    const uint16_t successor, const uint16_t selectorIndex, const common::Word suffix)
{
    if (successor != 0 and witnesses[successor] == common::Word{})
    {
        witnesses[successor] = witnesses[selectorIndex] + suffix;
    }
}

uint16_t AutomataGenerator::addSelector(const common::Word &selector)
{
    bool isAccepting{oracle.membershipQuery(selector)};
    selectors->addSelector(selector, isAccepting);
    return selectors->size() - 1;
}

uint16_t AutomataGenerator::findEquivalentSelector(
    const uint16_t selectorIndex, const uint16_t stackIndex, const uint16_t symbolIndex)
{
    for (uint16_t selector = 0; selector < selectors->size(); selector++)
    {
        if (isRightCongruence((*selectors)[selector], selectorIndex, stackIndex, symbolIndex))
        {
            return selector;
        }
    }

    return Selectors::INVALID_INDEX;
}

uint16_t AutomataGenerator::findEquivalentSelector(const common::Word &word)
{
    for (uint16_t selector = 0; selector < selectors->size(); selector++)
    {
        if (isRightCongruence((*selectors)[selector], word))
        {
            return selector;
        }
    }
    return Selectors::INVALID_INDEX;
}

bool AutomataGenerator::checkRightCongruenceWithEmptyControlLetters(
    const common::Word &a, const common::Word &b) const
{
    for (int i = 0; i < testWords->size(); i++)
    {
        if (std::get<Stack>((*testWords)[i][0]).top() != common::symbol::StackSymbol::BOTTOM)
        {
            continue;
        }

        common::Word w{(*testWords)[i]};
        w.erase(w.begin());

        if (areDistinguishable(a + w, b + (*testWords)[i]))
        {
            return false;
        }
    }
    return true;
}

bool AutomataGenerator::checkRightCongruenceWithReturnLetter(
    const common::Word &a, const uint16_t selectorIndex, const uint16_t stackIndex,
    const uint16_t symbolIndex) const
{
    for (int i = 0; i < testWords->size(); i++)
    {
        common::Stack controlLetter{std::get<Stack>((*testWords)[i][0])};
        common::Word word{(*testWords)[i]};
        word.erase(word.begin());

        controlLetter.push(common::symbol::StackSymbol{stackIndex});

        common::Word b{(*selectors)[selectorIndex]};
        b += common::Word{controlLetter};
        b += common::symbol::ReturnSymbol{symbolIndex};
        b += word;

        if (areDistinguishable(a + (*testWords)[i], b))
        {
            return false;
        }
    }
    return true;
}

bool AutomataGenerator::isRightCongruence(
    const common::Word &a, const uint16_t selectorIndex, const uint16_t stackIndex,
    const uint16_t symbolIndex) const
{
    if (stackIndex == common::symbol::StackSymbol::BOTTOM)
    {
        common::Word s{
            (*selectors)[selectorIndex] + common::Word{common::Stack{}} +
            common::Word{common::symbol::ReturnSymbol{symbolIndex}}};
        return checkRightCongruenceWithEmptyControlLetters(s, a);
    }
    else
    {
        return checkRightCongruenceWithReturnLetter(a, selectorIndex, stackIndex, symbolIndex);
    }
}

bool AutomataGenerator::isRightCongruence(const common::Word &a, const common::Word &b) const
{
    for (int i = 0; i < testWords->size(); i++)
    {
        if (areDistinguishable(a + (*testWords)[i], b + (*testWords)[i]))
        {
            return false;
        }
    }
    return true;
}

bool AutomataGenerator::areDistinguishable(const common::Word &a, const common::Word &b) const
{
    if (oracle.membershipQuery(a) != oracle.membershipQuery(b))
    {
        return true;
    }
    if (oracle.stackContentQuery(a) != oracle.stackContentQuery(b))
    {
        return true;
    }
    return false;
}

bool AutomataGenerator::isConfigurationAchievable(
    const uint16_t selectorIndex, const common::symbol::StackSymbol stackSymbol) const
{
    common::symbol::StackSymbol expectedStackTop{
        oracle.stackContentQuery(witnesses[selectorIndex]).top()};
    return expectedStackTop == stackSymbol;
}
} // namespace learner