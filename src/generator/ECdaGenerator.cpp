#include <algorithm>
#include <random>
#include <vector>

#include "generator/ECdaGenerator.hpp"
#include "utils/log.hpp"

namespace generator
{
void ECdaGenerator::validateGeneratorConfig()
{
    if (numOfStates < numOfModules)
    {
        ERR("[ECdaGenerator]: Number of states is lower than number of modules!");
        exit(2);
    }

    if (not(numOfCalls < numOfModules))
    {
        ERR("[ECdaGenerator]: Number of calls should be lower than number of modules!");
        exit(2);
    }

    if (numOfStackSymbols != numOfModules + 1) // TODO: numOfStackSymbols > numOfModules (require notification to Tester)
    {
        ERR("[ECdaGenerator]: Invalid number of stack symbols!");
        exit(2);
    }
}

void ECdaGenerator::selectTargetsForCalls()
{
    std::vector<uint16_t> randomModules(numOfModules - 1);
    std::iota(randomModules.begin(), randomModules.end(), 1);
    std::mt19937 g(rand());
    std::shuffle(randomModules.begin(), randomModules.end(), g);

    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        const auto module{randomModules[call]};
        const auto state{randRange(0, modules[module].size())};

        target[call] = common::transition::State{modules[module][state]};
    }
}

common::symbol::StackSymbol ECdaGenerator::selectStackSymbol(
    const uint16_t module, [[maybe_unused]] const uint16_t callId) const
{
    return static_cast<common::symbol::StackSymbol>(module + 1);
}

common::transition::State ECdaGenerator::findSuccessorForReturn(const uint16_t stackSymbol) const
{
    auto module{static_cast<uint16_t>(stackSymbol - 1)};
    return common::transition::State{modules[module][randRange(0, modules[module].size())]};
}
} // namespace generator