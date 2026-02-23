#include "generator/MeVpaGenerator.hpp"
#include "utils/log.hpp"
#include "utils/ExitCode.hpp"

namespace generator
{
void MeVpaGenerator::validateGeneratorConfig()
{
    if (numOfStates < numOfModules)
    {
        ERR("[MeVpaGenerator]: Number of states is lower than number of modules!");
        exit(toExit(ExitCode::GENERATOR));
    }

    if (numOfStackSymbols != numOfModules + 1)
    {
        ERR("[MeVpaGenerator]: Invalid number of stack symbols!");
        exit(toExit(ExitCode::GENERATOR));
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