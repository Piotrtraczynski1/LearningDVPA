#include "generator/XMLGenerator.hpp"
#include "utils/log.hpp"

namespace generator
{
bool XMLGenerator::generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis)
{
    return hypothesis->getNumOfStates() == 3;
}

std::shared_ptr<common::VPA> XMLGenerator::run()
{
    if (numOfCalls != numOfReturns or numOfCalls + 1 != numOfStackSymbols)
    {
        ERR("Generator parameters are incorrect: numOfCalls %u numOfReturns %u numOfStackSymbols: "
            "%u",
            numOfCalls, numOfReturns, numOfStackSymbols);
        exit(1);
    }

    generateTransition();
    return std::make_shared<common::VPA>(*transition, initialState, acceptingStates, numOfStates);
}

void XMLGenerator::generateTransition()
{
    // root
    transition->add(
        initialState, common::symbol::CallSymbol{0}, common::transition::State{1},
        common::symbol::StackSymbol{1});
    // back to root
    for (uint16_t stateId = 1; stateId < numOfStates; stateId++)
    {
        transition->add(
            common::transition::State{stateId}, common::symbol::StackSymbol{1},
            common::symbol::ReturnSymbol{0}, initialState);
    }

    // locals
    for (uint16_t localId = 0; localId < numOfLocals; localId++)
    {
        for (uint16_t stateId = 1; stateId < numOfStates; stateId++)
        {
            transition->add(
                common::transition::State{stateId}, common::symbol::LocalSymbol{localId},
                common::transition::State{stateId});
        }
    }

    // calls
    for (uint16_t callId = 1; callId < numOfCalls; callId++)
    {
        for (uint16_t stateId = 1; stateId < numOfStates; stateId++)
        {
            transition->add(
                common::transition::State{stateId}, common::symbol::CallSymbol{callId},
                common::transition::State{static_cast<uint16_t>((rand() % (numOfStates - 1)) + 1)},
                common::symbol::StackSymbol{static_cast<uint16_t>(callId + 1)});
        }
    }

    // returns
    for (uint16_t returnId = 1; returnId < numOfReturns; returnId++)
    {
        for (uint16_t stateId = 1; stateId < numOfStates; stateId++)
        {
            transition->add(
                common::transition::State{stateId},
                common::symbol::StackSymbol{static_cast<uint16_t>(returnId + 1)},
                common::symbol::ReturnSymbol{returnId},
                common::transition::State{static_cast<uint16_t>((rand() % (numOfStates - 1)) + 1)});
        }
    }
}
} // namespace generator
