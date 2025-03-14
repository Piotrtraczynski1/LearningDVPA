#include "generator/Generator.hpp"
#include "common/transition/State.hpp"

namespace generator
{
std::shared_ptr<common::VPA> Generator::run()
{
    generateTransition();
    auto acceptingStates{selectAcceptingStates()};
    common::transition::State initialState{0};
    return std::make_shared<common::VPA>(*transition, initialState, acceptingStates, numOfStates);
}

void Generator::generateTransition()
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

void Generator::addCalls(common::transition::State state)
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};
        common::symbol::StackSymbol stackSymbol{
            static_cast<uint16_t>((rand() % (numOfStackSymbols - 1)) + 1)};

        transition->add(state, common::symbol::CallSymbol{call}, dest, stackSymbol);
    }
}

void Generator::addLocals(common::transition::State state)
{
    for (uint16_t local = 0; local < numOfLocals; local++)
    {
        common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};

        transition->add(state, common::symbol::LocalSymbol{local}, dest);
    }
}

void Generator::addReturns(common::transition::State state)
{
    for (uint16_t ret = 0; ret < numOfReturns; ret++)
    {
        for (uint16_t stackSymbolId = 0; stackSymbolId < numOfStackSymbols; stackSymbolId++)
        {
            common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};
            common::symbol::StackSymbol stackSymbol{stackSymbolId};

            transition->add(state, stackSymbol, common::symbol::ReturnSymbol{ret}, dest);
        }
    }
}

std::vector<uint16_t> Generator::selectAcceptingStates()
{
    std::vector<uint16_t> acceptingStates{};
    acceptingStates.push_back(0);
    for (uint16_t i = 1; i < numOfStates; i++)
    {
        if (rand() % 3 == 0)
        {
            acceptingStates.push_back(i);
        }
    }

    return acceptingStates;
}
} // namespace generator