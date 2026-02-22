#include "generator/SeVpaGenerator.hpp"

namespace generator
{
void SeVpaGenerator::selectTargetsForCalls()
{
    selectEntryPoints();
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        auto module{randRange(1, numOfModules)};

        target[call] = common::transition::State{modules[module][entryPoints[module]]};
    }
}

void SeVpaGenerator::selectEntryPoints()
{
    for (uint16_t module = 1; module < numOfModules; module++)
    {
        entryPoints[module] = randRange(0, modules[module].size());
    }
}
} // namespace generator