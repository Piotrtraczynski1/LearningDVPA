#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesAndLocals.hpp"

namespace benchmark::scenario
{
std::string IncreasingNumOfStatesAndLocals::getDim2Name()
{
    return dim2Name;
}

uint16_t IncreasingNumOfStatesAndLocals::getDim2Details()
{
    return parameters.minNumOfLocals;
}

uint16_t IncreasingNumOfStatesAndLocals::getNumOfIterationsIn2Dim()
{
    return numOfIterationsIn2Dim;
}

void IncreasingNumOfStatesAndLocals::prepareNextIterationDim2()
{
    parameters.minNumOfLocals += 2;
    parameters.maxNumOfLocals += 2;
}

void IncreasingNumOfStatesAndLocals::resetDim2()
{
    parameters.minNumOfLocals = 2;
    parameters.maxNumOfLocals = 2;
}
} // namespace benchmark::scenario