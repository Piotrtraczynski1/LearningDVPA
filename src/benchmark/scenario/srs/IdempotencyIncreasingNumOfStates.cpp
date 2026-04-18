#include "benchmark/scenario/srs/IdempotencyIncreasingNumOfStates.hpp"
#include "Tester.hpp"
#include "generator/srs/IdempotencyGenerator.hpp"

namespace benchmark::scenario
{
uint16_t IdempotencyIncreasingNumOfStates::getNumOfIterationsIn1Dim()
{
    return numOfIterationsIn1Dim;
}

uint16_t IdempotencyIncreasingNumOfStates::getDim1Details()
{
    return parameters.minNumOfStates;
}

void IdempotencyIncreasingNumOfStates::runSingleIteration()
{
    Tester{
        numOfTestsInSingleIteration,
        std::unique_ptr<generator::Generator>(new generator::srs::IdempotencyGenerator()),
        parameters}
        .run();
}
} // namespace benchmark::scenario