#include <algorithm>
#include <random>
#include <vector>

#include "generator/ECdaGenerator.hpp"
#include "utils/ExitCode.hpp"
#include "utils/log.hpp"

namespace generator
{
void ECdaGenerator::validateGeneratorConfig(
    uint16_t &numOfStates_, uint16_t &numOfCalls_, [[maybe_unused]] uint16_t &numOfLocals_,
    [[maybe_unused]] uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_)
{
    if (numOfStates < numOfModules)
    {
        if (numOfModules > utils::MaxNumOfAutomatonStates)
        {
            ERR("[ECdaGenerator]: numOfModules (%u) is greater than MaxNumOfAutomatonStates!",
                numOfModules);
            exit(toExit(ExitCode::GENERATOR));
        }
        WRN("[ECdaGenerator] numOfStates (%u) is less than numOfModules (%u). Adjusting "
            "numOfStates "
            "to %u",
            numOfStates, numOfModules, numOfModules);
        numOfStates_ = numOfModules;
        numOfStates = numOfModules;
    }

    if (not(numOfCalls < numOfModules))
    {
        const uint16_t validNumOfCalls{static_cast<uint16_t>(numOfModules - 1)};
        WRN("[ECdaGenerator]: numOfCalls (%u) should be lower than number of modules (%u)! "
            "Adjusting numOfCalls to %u",
            numOfCalls, numOfModules, validNumOfCalls);
        numOfCalls_ = validNumOfCalls;
        numOfCalls = validNumOfCalls;
    }

    if (numOfStackSymbols != numOfModules + 1)
    {
        const uint16_t expectedNumOfStackSymbols{static_cast<uint16_t>(numOfModules + 1)};
        if (expectedNumOfStackSymbols > utils::MaxNumOfStackSymbols)
        {
            ERR("[ECdaGenerator]: expectedNumOfStackSymbols (%u) is greater than "
                "MaxNumOfStackSymbols!",
                expectedNumOfStackSymbols);
            exit(toExit(ExitCode::GENERATOR));
        }
        WRN("[ECdaGenerator]: numOfStackSymbols (%u) should be equal numOfModules (%u)"
            " + 1. Adjusting numOfStackSymbols to: %u",
            numOfStackSymbols, numOfModules, expectedNumOfStackSymbols);
        numOfStackSymbols_ = expectedNumOfStackSymbols;
        numOfStackSymbols = expectedNumOfStackSymbols;
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