#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesAndStackSymbols.hpp"

namespace benchmark::scenario
{
std::string IncreasingNumOfStatesAndStackSymbols::getDim2Name()
{
    return dim2Name;
}

uint16_t IncreasingNumOfStatesAndStackSymbols::getDim2Details()
{
    return parameters.minNumOfStackSymbols;
}

uint16_t IncreasingNumOfStatesAndStackSymbols::getNumOfIterationsIn2Dim()
{
    return numOfIterationsIn2Dim;
}

void IncreasingNumOfStatesAndStackSymbols::prepareNextIterationDim2()
{
    parameters.minNumOfStackSymbols += 2;
    parameters.maxNumOfStackSymbols += 2;
}

void IncreasingNumOfStatesAndStackSymbols::resetDim2()
{
    parameters.minNumOfStackSymbols = 5;
    parameters.maxNumOfStackSymbols = 5;
}
} // namespace benchmark::scenario