#include "generator/Generator.hpp"

namespace generator
{
void Generator::setConfig(
    const uint16_t numOfStates_, const uint16_t numOfCalls_, const uint16_t numOfLocals_,
    const uint16_t numOfReturns_, const uint16_t numOfStackSymbols_, const double density_,
    const double acceptingStatesDensity_, const uint16_t numOfModules_)
{
    numOfStates = numOfStates_;
    numOfCalls = numOfCalls_;
    numOfLocals = numOfLocals_;
    numOfReturns = numOfReturns_;
    numOfStackSymbols = numOfStackSymbols_;
    density = density_;
    acceptingStatesDensity = acceptingStatesDensity_;
    numOfModules = numOfModules_;

    transition = std::make_shared<common::transition::Transition<AutomatonKind::Normal>>();
}

bool Generator::generatorSpecificCheck(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    return numOfStates + 1 >= hypothesis->getNumOfStates(); // +1 for explicit sink state
}

learner::srs::Srs Generator::getSrs()
{
    return learner::srs::Srs{};
}

void Generator::selectAcceptingStates()
{
    for (uint16_t i = 0; i < numOfStates; i++)
    {
        if (shouldAccept())
        {
            acceptingStates.push_back(i);
        }
    }
}

bool Generator::skipTransition()
{
    return ((static_cast<double>(rand()) / RAND_MAX) >= density);
}

bool Generator::shouldAccept()
{
    return ((static_cast<double>(rand()) / RAND_MAX) < acceptingStatesDensity);
}
} // namespace generator