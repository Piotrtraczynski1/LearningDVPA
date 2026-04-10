#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesAndCalls.hpp"

namespace benchmark::scenario
{
std::string IncreasingNumOfStatesAndCalls::getDim2Name()
{
    return dim2Name;
}

uint16_t IncreasingNumOfStatesAndCalls::getDim2Details()
{
    return parameters.minNumOfCalls;
}

uint16_t IncreasingNumOfStatesAndCalls::getNumOfIterationsIn2Dim()
{
    return numOfIterationsIn2Dim;
}

void IncreasingNumOfStatesAndCalls::prepareNextIterationDim2()
{
    parameters.minNumOfCalls += 2;
    parameters.maxNumOfCalls += 2;
}

void IncreasingNumOfStatesAndCalls::resetDim2()
{
    parameters.minNumOfCalls = 3;
    parameters.maxNumOfCalls = 3;
}
} // namespace benchmark::scenario