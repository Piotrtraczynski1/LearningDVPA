#include "benchmark/scenario/srs/BaseSrsScenario.hpp"
#include "Tester.hpp"
#include "generator/RandomGenerator.hpp"

namespace benchmark::scenario
{
int BaseSrsScenario::getSeed()
{
    return seed;
}

uint16_t BaseSrsScenario::getNumOfIterationsIn2Dim()
{
    return numOfIterationsIn2Dim;
}

uint16_t BaseSrsScenario::getNumOfTestsInSingleIteration()
{
    return numOfTestsInSingleIteration;
}

void BaseSrsScenario::prepareNextIterationDim2()
{
    std::srand(getSeed());
    parameters.useSrs = true;
}

void BaseSrsScenario::resetDim2()
{
    parameters.useSrs = false;
}

std::string BaseSrsScenario::getDim2Name()
{
    return dim2Name;
}

uint16_t BaseSrsScenario::getDim2Details()
{
    return parameters.useSrs;
}

void BaseSrsScenario::prepareNextIterationDim1()
{
    parameters.minNumOfStates += 1;
    parameters.maxNumOfStates += 1;
}

void BaseSrsScenario::resetDim1()
{
    parameters.minNumOfStates = 2;
    parameters.maxNumOfStates = 2;
}

std::string BaseSrsScenario::getDim1Name()
{
    return dim1Name;
}
} // namespace benchmark::scenario