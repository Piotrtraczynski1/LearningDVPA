#include "generator/srs/IdempotencyGenerator.hpp"
#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"

namespace generator::srs
{
learner::srs::Srs IdempotencyGenerator::getSrs()
{
    learner::srs::Srs srs{};
    for (uint16_t callId = 0; callId < numOfCalls; callId++)
    {
        const common::Word callSymbol{common::symbol::CallSymbol{callId}};
        for (uint16_t returnId = 0; returnId < numOfReturns; returnId++)
        {
            const common::Word returnSymbol{common::symbol::ReturnSymbol{returnId}};
            srs.push_back(
                learner::srs::SrsRuleWithParams{
                    .left = {.left = callSymbol + callSymbol, .right = returnSymbol + returnSymbol},
                    .right = {.left = callSymbol, .right = returnSymbol}});
        }
    }

    return srs;
}

void IdempotencyGenerator::addReturns()
{
    for (uint16_t it = 0; it < modules.size(); it++)
    {
        for (auto state : modules[it])
        {
            const common::transition::State currentState{state};
            for (uint16_t ret = 0; ret < numOfReturns; ret++)
            {
                for (uint16_t stackSymbol = 1; stackSymbol < numOfStackSymbols; stackSymbol++)
                {
                    const auto [moduleIdx, _] = decodeStackSymbol(stackSymbol);
                    if (moduleIdx == it)
                    {
                        transition->add(
                            currentState, common::symbol::StackSymbol{stackSymbol},
                            common::symbol::ReturnSymbol{ret}, currentState);
                        continue;
                    }

                    if (skipTransition())
                    {
                        continue;
                    }

                    transition->add(
                        currentState, common::symbol::StackSymbol{stackSymbol},
                        common::symbol::ReturnSymbol{ret}, findSuccessorForReturn(stackSymbol));
                }
            }
        }
    }
}

void IdempotencyGenerator::addCalls()
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        for (uint16_t module = 0; module < modules.size(); module++)
        {
            for (auto state : modules[module])
            {
                transition->add(
                    common::transition::State{state}, common::symbol::CallSymbol{call},
                    target[call], selectStackSymbol(module, call));
            }
        }
    }
}
} // namespace generator::srs