#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesBase.hpp"
#include "Tester.hpp"
#include "generator/RandomGenerator.hpp"

namespace benchmark::scenario
{
int IncreasingNumOfStatesBase::getSeed()
{
    return seed;
}

uint16_t IncreasingNumOfStatesBase::getNumOfIterations()
{
    return numOfIterations;
}

uint16_t IncreasingNumOfStatesBase::getNumOfTestsInSingleIteration()
{
    return numOfTestsInSingleIteration;
}

void IncreasingNumOfStatesBase::runSingleIteration()
{
    Tester{
        numOfTestsInSingleIteration,
        std::unique_ptr<generator::Generator>(new generator::RandomGenerator()), parameters}
        .run();
    prepareNextIteration();
}

void IncreasingNumOfStatesBase::prepareNextIteration()
{
    parameters.minNumOfStates++;
    parameters.maxNumOfStates++;
}
} // namespace benchmark::scenario