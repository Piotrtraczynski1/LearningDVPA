#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesAndReturns.hpp"

namespace benchmark::scenario
{
std::string IncreasingNumOfStatesAndReturns::getDim2Name()
{
    return dim2Name;
}

uint16_t IncreasingNumOfStatesAndReturns::getDim2Details()
{
    return parameters.minNumOfReturns;
}

uint16_t IncreasingNumOfStatesAndReturns::getNumOfIterationsIn2Dim()
{
    return numOfIterationsIn2Dim;
}

void IncreasingNumOfStatesAndReturns::prepareNextIterationDim2()
{
    parameters.minNumOfReturns += 2;
    parameters.maxNumOfReturns += 2;
}

void IncreasingNumOfStatesAndReturns::resetDim2()
{
    parameters.minNumOfReturns = 2;
    parameters.maxNumOfReturns = 2;
}
} // namespace benchmark::scenario