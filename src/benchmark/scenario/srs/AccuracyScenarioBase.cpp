#include "benchmark/scenario/srs/AccuracyScenarioBase.hpp"

namespace benchmark::scenario
{
int AccuracyScenarioBase::getSeed()
{
    return seed;
}

uint16_t AccuracyScenarioBase::getNumOfTestsInSingleIteration()
{
    return numOfTestsInSingleIteration;
}

uint16_t AccuracyScenarioBase::getNumOfIterationsIn1Dim()
{
    return 1;
}

void AccuracyScenarioBase::prepareNextIterationDim1()
{
    // intentionally undefined
}

void AccuracyScenarioBase::resetDim1()
{
    // intentionally undefined
}

std::string AccuracyScenarioBase::getDim1Name()
{
    return dim1Name;
}
} // namespace benchmark::scenario