#include "generator/MeVpaGenerator.hpp"
#include "utils/ExitCode.hpp"
#include "utils/log.hpp"

namespace generator
{

void MeVpaGenerator::validateGeneratorConfig(
    uint16_t &numOfStates_, [[maybe_unused]] uint16_t &numOfCalls_,
    [[maybe_unused]] uint16_t &numOfLocals_, [[maybe_unused]] uint16_t &numOfReturns_,
    uint16_t &numOfStackSymbols_)
{
    if (numOfStates < numOfModules)
    {
        if (numOfModules > utils::MaxNumOfAutomatonStates)
        {
            ERR("[MeVpaGenerator]: numOfModules (%u) is greater than MaxNumOfAutomatonStates!",
                numOfModules);
            exit(toExit(ExitCode::GENERATOR));
        }
        WRN("[MeVpaGenerator] numOfStates (%u) is less than numOfModules (%u). Adjusting "
            "numOfStates "
            "to %u",
            numOfStates, numOfModules, numOfModules);
        numOfStates_ = numOfModules;
        numOfStates = numOfModules;
    }
    if (numOfStackSymbols != numOfModules + 1)
    {
        const uint16_t expectedNumOfStackSymbols{static_cast<uint16_t>(numOfModules + 1)};
        if (expectedNumOfStackSymbols > utils::MaxNumOfStackSymbols)
        {
            ERR("[MeVpaGenerator]: expectedNumOfStackSymbols (%u) is greater than "
                "MaxNumOfStackSymbols!",
                expectedNumOfStackSymbols);
            exit(toExit(ExitCode::GENERATOR));
        }
        WRN("[MeVpaGenerator]: numOfStackSymbols (%u) should be equal numOfModules (%u)"
            " + 1. Adjusting numOfStackSymbols to: %u",
            numOfStackSymbols, numOfModules, expectedNumOfStackSymbols);
        numOfStackSymbols_ = expectedNumOfStackSymbols;
        numOfStackSymbols = expectedNumOfStackSymbols;
    }
}

common::symbol::StackSymbol MeVpaGenerator::selectStackSymbol(
    const uint16_t module, [[maybe_unused]] const uint16_t callId) const
{
    return static_cast<common::symbol::StackSymbol>(module + 1);
}

common::transition::State MeVpaGenerator::findSuccessorForReturn(const uint16_t stackSymbol) const
{
    auto module{static_cast<uint16_t>(stackSymbol - 1)};
    return common::transition::State{modules[module][randRange(0, modules[module].size())]};
}
} // namespace generator