#include <algorithm>
#include <numeric>
#include <random>

#include "generator/eCDAGenerator.hpp"

namespace generator
{
bool eCDAGenerator::generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis)
{
    return true; // TODO
}

std::shared_ptr<common::VPA> eCDAGenerator::run()
{
    clear();
    splitStates();
    selectStackSymbolsToModules();

    addLocals();
    addCalls();
    addReturns();
    selectAcceptingStates();

    return std::make_shared<common::VPA>(*transition, initialState, acceptingStates, numOfStates);
}

learner::Srs eCDAGenerator::getSrs()
{
    return srs;
}

void eCDAGenerator::clear()
{
    srs.clear();
    transition->clear();
    splitedStates.clear();
    splitedStates.resize(numOfModules);
    acceptingStates.clear();
    stackSymbolsToModules.clear();
    modulesToStackSymbols.clear();
    takenSuccessors.clear();
    takenSuccessors.insert(initialState);
}

void eCDAGenerator::addLocals()
{
    for (auto module : splitedStates)
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

                for (uint16_t callId = 0; callId < numOfCalls; callId++)
                {
                    common::Word lhs{
                        common::Word{common::symbol::LocalSymbol{local}} +
                        common::Word{common::symbol::CallSymbol{callId}}};
                    common::Word rhs{common::Word{common::symbol::CallSymbol{callId}}};

                    srs.push_back({lhs, rhs});
                }
            }
        }
    }
}

void eCDAGenerator::addCalls()
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        auto dest{selectCallSuccessor()};
        for (uint16_t module = 0; module < splitedStates.size(); module++)
        {
            for (auto state : splitedStates[module])
            {
                if (skipTransition())
                {
                    continue;
                }

                transition->add(
                    common::transition::State{state}, common::symbol::CallSymbol{call}, dest,
                    selectStackSymbolForModule(module));
            }
        }
    }
}

void eCDAGenerator::addReturns()
{
    for (auto module : splitedStates)
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

void eCDAGenerator::selectAcceptingStates()
{
    for (uint16_t i = 0; i < numOfStates; i++)
    {
        if (shouldAccept())
        {
            acceptingStates.push_back(i);
        }
    }
}

common::transition::State eCDAGenerator::selectCallSuccessor()
{
    uint16_t successor;
    do
    {
        successor = rand() % numOfStates;
    } while (takenSuccessors.find(successor) != takenSuccessors.end());
    takenSuccessors.insert(successor);
    return common::transition::State{successor};
}

common::symbol::StackSymbol eCDAGenerator::selectStackSymbolForModule(const uint16_t module)
{
    return common::symbol::StackSymbol{
        modulesToStackSymbols[module][(rand() % modulesToStackSymbols[module].size())]};
}

common::transition::State eCDAGenerator::findSuccessorForReturn(const uint16_t stackSymbol)
{
    const auto &module{stackSymbolsToModules.at(stackSymbol)};
    return common::transition::State{
        splitedStates[module][(rand() % splitedStates[module].size())]};
}

void eCDAGenerator::splitStates()
{
    std::vector<uint16_t> splitPoints{selectSplitPoints()};
    uint16_t state{0};
    for (uint16_t it = 0; it < splitPoints.size(); it++)
    {
        while (state <= splitPoints[it])
        {
            splitedStates[it].push_back(state);
            state++;
        }
    }

    while (state < numOfStates)
    {
        splitedStates[splitPoints.size()].push_back(state);
        state++;
    }
}

void eCDAGenerator::selectStackSymbolsToModules()
{
    uint16_t modulesWithStackSymbol{0};
    for (uint16_t stackSymbol = 1; stackSymbol < numOfStackSymbols; stackSymbol++)
    {
        if (modulesWithStackSymbol >= numOfModules)
        {
            uint16_t module{static_cast<uint16_t>(rand() % numOfModules)};

            stackSymbolsToModules.emplace(stackSymbol, module);
            modulesToStackSymbols[module].push_back(stackSymbol);
        }
        else
        {
            stackSymbolsToModules.emplace(stackSymbol, modulesWithStackSymbol);
            modulesToStackSymbols.push_back(std::vector<uint16_t>{stackSymbol});
            modulesWithStackSymbol++;
        }
    }
}

std::vector<uint16_t> eCDAGenerator::selectSplitPoints()
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