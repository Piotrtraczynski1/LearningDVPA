#include <algorithm>
#include <numeric>
#include <random>

#include "generator/CdaGenerator.hpp"
#include "utils/log.hpp"

namespace generator
{
void CdaGenerator::validateGeneratorConfig()
{
    if (numOfStates < numOfModules)
    {
        ERR("[CdaGenerator]: Number of states is lower than number of modules!");
        exit(2);
    }

    if (numOfStackSymbols != numOfModules * numOfCalls + 1)
    {
        ERR("[CdaGenerator]: Invalid number of stack symbols!");
        exit(2);
    }
}

uint16_t CdaGenerator::randRange(const uint16_t a, const uint16_t b) const
{
    return a + (rand() % (b - a));
}

std::shared_ptr<common::VPA> CdaGenerator::run()
{
    validateGeneratorConfig();
    clear();
    selectModules();
    selectTargetsForCalls();

    generateTransition();
    selectAcceptingStates();
    return std::make_shared<common::VPA>(*transition, initialState, acceptingStates, numOfStates);
}

void CdaGenerator::clear()
{
    transition->clear();
    modules.clear();
    modules.resize(numOfModules);
    acceptingStates.clear();
}

void CdaGenerator::generateTransition()
{
    addLocals();
    addCalls();
    addReturns();
}

void CdaGenerator::addLocals()
{
    for (auto module : modules)
    {
        for (auto state : module)
        {
            for (uint16_t local = 0; local < numOfLocals; local++)
            {
                if (skipTransition())
                {
                    continue;
                }

                transition->add(
                    common::transition::State{state}, common::symbol::LocalSymbol{local},
                    common::transition::State{module[(rand() % module.size())]});
            }
        }
    }
}

void CdaGenerator::addCalls()
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        for (uint16_t module = 0; module < modules.size(); module++)
        {
            for (auto state : modules[module])
            {
                if (skipTransition())
                {
                    continue;
                }

                transition->add(
                    common::transition::State{state}, common::symbol::CallSymbol{call},
                    target[call], selectStackSymbol(module, call));
            }
        }
    }
}

void CdaGenerator::addReturns()
{
    for (auto module : modules)
    {
        for (auto state : module)
        {
            for (uint16_t ret = 0; ret < numOfReturns; ret++)
            {
                for (uint16_t stackSymbol = 1; stackSymbol < numOfStackSymbols; stackSymbol++)
                {
                    if (skipTransition())
                    {
                        continue;
                    }

                    transition->add(
                        common::transition::State{state}, common::symbol::StackSymbol{stackSymbol},
                        common::symbol::ReturnSymbol{ret}, findSuccessorForReturn(stackSymbol));
                }
            }
        }
    }
}

common::transition::State CdaGenerator::findSuccessorForReturn(const uint16_t stackSymbol) const
{
    const auto [moduleIdx, _] = decodeStackSymbol(stackSymbol);
    const auto module{modules[moduleIdx]};

    return common::transition::State{module[randRange(0, module.size())]};
}

common::symbol::StackSymbol
CdaGenerator::selectStackSymbol(const uint16_t module, const uint16_t callId) const
{
    return encodeStackSymbol(module, callId);
}

common::symbol::StackSymbol
CdaGenerator::encodeStackSymbol(const uint16_t module, const uint16_t callId) const
{
    return static_cast<common::symbol::StackSymbol>(module * numOfCalls + callId + 1);
}

std::pair<uint16_t, uint16_t> CdaGenerator::decodeStackSymbol(uint16_t stackSymbol) const
{
    stackSymbol--;
    return std::make_pair<uint16_t, uint16_t>(
        (stackSymbol / numOfCalls), (stackSymbol % numOfCalls));
}

void CdaGenerator::selectTargetsForCalls()
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        auto module{randRange(1, numOfModules)};
        auto state{randRange(0, modules[module].size())};

        target[call] = common::transition::State{modules[module][state]};
    }
}

void CdaGenerator::selectModules()
{
    std::vector<uint16_t> splitPoints{selectSplitPoints()};
    uint16_t state{0};
    for (uint16_t it = 0; it < numOfModules - 1; it++)
    {
        while (state <= splitPoints[it])
        {
            stateToModule[state] = it;
            modules[it].push_back(state);
            state++;
        }
    }

    while (state < numOfStates)
    {
        stateToModule[state] = numOfModules - 1;
        modules[numOfModules - 1].push_back(state);
        state++;
    }
}

std::vector<uint16_t> CdaGenerator::selectSplitPoints()
{
    std::vector<uint16_t> states(numOfStates - 1);
    std::iota(states.begin(), states.end(), 0);
    std::mt19937 g(rand());
    std::shuffle(states.begin(), states.end(), g);
    states.resize(numOfModules - 1);
    std::sort(states.begin(), states.end());
    return states;
}
} // namespace generator