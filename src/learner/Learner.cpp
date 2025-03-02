#include "Learner.hpp"

namespace learner
{
void Learner::run()
{
    std::shared_ptr<common::Word> counterExample;
    while (true)
    {
        generateAutomata();
        counterExample = oracle.equivalenceQuery(*hypothesis); // pass pointer instead of value

        if (counterExample->empty())
        {
            std::cout << "VPA found" << std::endl;
            return;
        }
        handleCounterExample(
            counterExample); // new_State, new_test_word powinniśmy otrzymać do S i T.
    }
}

template <typename Symbol> // To chyba w miare git iddea jest taka, że jak nie znajdziemy to
                           // dodajemy nowy stan :)
                           uint16_t Learner::findEquivalentState(uint16_t stateIndex, Symbol sym,
                                                                 uint16_t stackIndex)
{
    common::Word suffix{common::ControlWord{common::symbol::StackSymbol{stackIndex}}, Symbol{sym}};
    common::Word candidate{selectors[stateIndex] + suffix};
    for (uint16_t state = 0; state < selectors.size(); state++)
    {
        if (isRightCongruence(selectors[state], candidate))
        {
            return state;
        }
    }
    bool isAccepting{oracle.membershipQuery(candidate)};
    selectors.addSelector(candidate, isAccepting);
    return selectors.size() - 1;
}

void Learner::generateAutomata() /* DRAFT !! */
{
    for (uint16_t stateIndex = 0; stateIndex < selectors.size(); stateIndex++)
    {
        for (uint16_t stackIndex = 0; stackIndex < numOfStackSymbols; stackIndex++)
        {
            for (uint16_t symbolIndex = 0; symbolIndex < numOfCalls; symbolIndex++)
            {
                common::transition::Argument<common::symbol::CallSymbol> arg{
                    common::symbol::CallSymbol{symbolIndex},
                    common::symbol::StackSymbol{stackIndex}, selectors.getState(stackIndex)};

                common::transition::CoArgument coArg{
                    common::symbol::StackSymbol{
                        stackIndex}, // how can we know which symbol should be added to stack ???
                    selectors.getState(findEquivalentState(
                        stateIndex, common::symbol::CallSymbol{symbolIndex}, stackIndex))};

                callT[arg] = coArg;
            }
            for (uint16_t symbolIndex = 0; symbolIndex < numOfReturns; symbolIndex++)
            {
                common::transition::Argument<common::symbol::ReturnSymbol> arg{
                    common::symbol::ReturnSymbol{symbolIndex},
                    common::symbol::StackSymbol{stackIndex}, selectors.getState(stackIndex)};

                common::transition::State coArg{selectors.getState(findEquivalentState(
                    stateIndex, common::symbol::ReturnSymbol{symbolIndex}, stackIndex))};

                returnT[arg] = coArg;
            };
            for (uint16_t symbolIndex = 0; symbolIndex < numOfLocals; symbolIndex++)
            {
                common::transition::Argument<common::symbol::LocalSymbol> arg{
                    common::symbol::LocalSymbol{symbolIndex},
                    common::symbol::StackSymbol{stackIndex}, selectors.getState(stackIndex)};

                common::transition::State coArg{selectors.getState(findEquivalentState(
                    stateIndex, common::symbol::LocalSymbol{symbolIndex}, stackIndex))};

                localT[arg] = coArg;
            }
        }
    }
}

void Learner::handleCounterExample(std::shared_ptr<common::Word> counterExample)
{
    for (const auto symbol : *counterExample)
    {
    }
}

bool Learner::isRightCongruence(common::Word &a, common::Word &b)
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

bool Learner::areDistinguishable(const common::Word &a, const common::Word &b)
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