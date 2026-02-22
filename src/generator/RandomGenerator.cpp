#include "generator/RandomGenerator.hpp"
#include "common/transition/State.hpp"

namespace generator
{
std::shared_ptr<common::VPA> RandomGenerator::run()
{
    generateTransition();
    selectAcceptingStates();
    return std::make_shared<common::VPA>(*transition, initialState, acceptingStates, numOfStates);
}

void RandomGenerator::generateTransition()
{
    transition->clear();

    for (uint16_t stateId = 0; stateId < numOfStates; stateId++)
    {
        common::transition::State state{stateId};
        addCalls(state);
        addLocals(state);
        addReturns(state);
    }
}

void RandomGenerator::addCalls(common::transition::State state)
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        if (skipTransition())
        {
            continue;
        }
        common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};
        common::symbol::StackSymbol stackSymbol{
            static_cast<uint16_t>((rand() % (numOfStackSymbols - 1)) + 1)};

        transition->add(state, common::symbol::CallSymbol{call}, dest, stackSymbol);
    }
}

void RandomGenerator::addLocals(common::transition::State state)
{
    for (uint16_t local = 0; local < numOfLocals; local++)
    {
        if (skipTransition())
        {
            continue;
        }
        common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};

        transition->add(state, common::symbol::LocalSymbol{local}, dest);
    }
}

void RandomGenerator::addReturns(common::transition::State state)
{
    for (uint16_t ret = 0; ret < numOfReturns; ret++)
    {
        for (uint16_t stackSymbolId = 0; stackSymbolId < numOfStackSymbols; stackSymbolId++)
        {
            if (skipTransition())
            {
                continue;
            }
            common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};
            common::symbol::StackSymbol stackSymbol{stackSymbolId};

            transition->add(state, stackSymbol, common::symbol::ReturnSymbol{ret}, dest);
        }
    }
}
} // namespace generator