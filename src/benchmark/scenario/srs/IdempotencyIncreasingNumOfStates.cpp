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

std::string IdempotencyIncreasingNumOfStates::getGeneratorName() const
{
    return "idempotency";
}

SingleTestResult IdempotencyIncreasingNumOfStates::runSingle(uint32_t seed)
{
    return Tester{1, std::make_unique<generator::srs::IdempotencyGenerator>(), parameters, seed}
        .runSingle();
}

void IdempotencyIncreasingNumOfStates::prepareNextIterationDim1()
{
    parameters.minNumOfStates += 3;
    parameters.maxNumOfStates += 3;
}
} // namespace benchmark::scenario
