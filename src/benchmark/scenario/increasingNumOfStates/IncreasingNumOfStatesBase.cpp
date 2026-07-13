#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesBase.hpp"
#include "Tester.hpp"
#include "generator/RandomGenerator.hpp"

namespace benchmark::scenario
{
int IncreasingNumOfStatesBase::getSeed()
{
    return seed;
}

uint16_t IncreasingNumOfStatesBase::getNumOfIterationsIn1Dim()
{
    return numOfIterationsIn1Dim;
}

std::string IncreasingNumOfStatesBase::getDim1Name()
{
    return dim1Name;
}

uint16_t IncreasingNumOfStatesBase::getDim1Details()
{
    return parameters.minNumOfStates;
}

uint16_t IncreasingNumOfStatesBase::getNumOfTestsInSingleIteration()
{
    return numOfTestsInSingleIteration;
}

const TesterParameters &IncreasingNumOfStatesBase::getParameters() const
{
    return parameters;
}

std::string IncreasingNumOfStatesBase::getGeneratorName() const
{
    return "random";
}

SingleTestResult IncreasingNumOfStatesBase::runSingle(uint32_t seed)
{
    return Tester{1, std::make_unique<generator::RandomGenerator>(), parameters, seed}.runSingle();
}

void IncreasingNumOfStatesBase::prepareNextIterationDim1()
{
    parameters.minNumOfStates += 2;
    parameters.maxNumOfStates += 2;
}

void IncreasingNumOfStatesBase::resetDim1()
{
    parameters.minNumOfStates = 2;
    parameters.maxNumOfStates = 2;
}
} // namespace benchmark::scenario
