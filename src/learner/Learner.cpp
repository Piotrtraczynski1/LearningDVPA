#include <iostream> //tmp

#include "learner/Learner.hpp"
#include "utils/log.hpp"

namespace learner
{
std::shared_ptr<common::VPA> Learner::run()
{
    std::shared_ptr<common::Word> counterExample;
    while (true)
    {
        generateAutomata();
        LOG("[Learner]: Automata generated\n");

        common::transition::Transition tran{callT, returnT, localT};
        hypothesis = std::make_shared<common::VPA>(common::VPA{tran, selectors.getState(0)});

        counterExample = oracle.equivalenceQuery(hypothesis);

        if (counterExample->empty())
        {
            LOG("[Learner]: VPA found\n");
            hypothesis->print();

            return hypothesis;
        }
        handleCounterExample(counterExample);
    }
}

uint16_t Learner::findEquivalentState(const common::Word word)
{
    for (uint16_t state = 0; state < selectors.size(); state++)
    {
        if (isRightCongruence(selectors[state], word))
        {
            return state;
        }
    }

    LOG("[Learner]: Adding new state: ");
    std::cout << word << std::endl;

    bool isAccepting{oracle.membershipQuery(word)};
    selectors.addSelector(word, isAccepting);
    return selectors.size() - 1;
}

void Learner::generateAutomata() /* DRAFT */
{
    LOG("[Learner]: Generating Automata\n");
    LOG("numOfCall=%d, numOfReturns=%d, numOfLocals=%d\n", numOfCalls, numOfReturns, numOfLocals);
    for (uint16_t stateIndex = 0; stateIndex < selectors.size(); stateIndex++)
    {
        for (uint16_t stackIndex = 0; stackIndex <= numOfStackSymbols; stackIndex++)
        {
            if (stackIndex == numOfStackSymbols)
            {
                stackIndex = static_cast<uint16_t>(common::symbol::StackSymbol::BOTTOM);
            }
            for (uint16_t symbolIndex = 0; symbolIndex < numOfCalls; symbolIndex++)
            {
                common::transition::Argument<common::symbol::CallSymbol> arg{
                    common::symbol::CallSymbol{symbolIndex},
                    common::symbol::StackSymbol{stackIndex}, selectors.getState(stateIndex)};

                common::transition::CoArgument coArg{
                    common::symbol::StackSymbol{stackIndex}, // --- StackContentQuery!!!!
                    selectors.getState(findEquivalentState(
                        selectors[stateIndex] +
                        common::Word{common::symbol::CallSymbol{symbolIndex}}))};

                callT[arg] = coArg;
            }
            for (uint16_t symbolIndex = 0; symbolIndex < numOfReturns; symbolIndex++)
            {
                common::transition::Argument<common::symbol::ReturnSymbol> arg{
                    common::symbol::ReturnSymbol{symbolIndex},
                    common::symbol::StackSymbol{stackIndex}, selectors.getState(stateIndex)};

                common::transition::State coArg{selectors.getState(findEquivalentState(
                    selectors[stateIndex] +
                    common::Word{common::symbol::ReturnSymbol{symbolIndex}}))};

                returnT[arg] = coArg;
            };
            for (uint16_t symbolIndex = 0; symbolIndex < numOfLocals; symbolIndex++)
            {
                common::transition::Argument<common::symbol::LocalSymbol> arg{
                    common::symbol::LocalSymbol{symbolIndex},
                    common::symbol::StackSymbol{stackIndex}, selectors.getState(stateIndex)};

                common::transition::State coArg{selectors.getState(findEquivalentState(
                    selectors[stateIndex] +
                    common::Word{common::symbol::LocalSymbol{symbolIndex}}))};

                localT[arg] = coArg;
            }
            // TODO Remove
            if (stackIndex == 0xffff)
                stackIndex = numOfStackSymbols + 1;
        }
    }
}

// To be change
bool Learner::isAppropriateSplit(const common::Word &word) const
{
    std::cout << "[Learner]: Word: " << word << std::endl;
    LOG("[Learner]: isAppropriateSlipt: hyp.mem=%d, oracle.mem=%d\n", hypothesis->checkWord(word),
        oracle.membershipQuery(word));
    if (hypothesis->checkWord(word) != oracle.membershipQuery(word))
    {
        return true;
    }
    if (hypothesis->stack != oracle.stackContentQuery(word))
    {
        return true;
    }
    return false;
}

void Learner::handleCounterExample(std::shared_ptr<common::Word> counterExample)
{
    std::cout << "[Learner]: CounterExample: " << *counterExample << std::endl;
    hypothesis->print();
    common::Word candidate{};
    for (uint16_t it = 0; it < counterExample->size(); it++)
    {
        candidate += (*counterExample)[it];
        if (isAppropriateSplit(candidate))
        {
            selectors.addSelector(candidate, oracle.membershipQuery(candidate));
            std::cout << "[Learner]: adding prefix: " << candidate << std::endl;
            common::Word suffix{oracle.stackContentQuery(candidate)};
            suffix.insert(suffix.end(), counterExample->begin() + it, counterExample->end());
            testWords.addWord(suffix);
            std::cout << "[Learner]: adding suffix: " << suffix << std::endl;
            return;
        }
    }
}

bool Learner::isRightCongruence(const common::Word &a, const common::Word &b) const
{
    for (int i = 0; i < testWords.size(); i++)
    {
        if (areDistinguishable(a + testWords[i], b + testWords[i]))
        {
            return false;
        }
    }
    return true;
}

bool Learner::areDistinguishable(const common::Word &a, const common::Word &b) const
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
} // namespace learner