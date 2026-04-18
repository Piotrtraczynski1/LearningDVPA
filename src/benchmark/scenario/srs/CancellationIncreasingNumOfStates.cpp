#include "benchmark/scenario/srs/CancellationIncreasingNumOfStates.hpp"
#include "Tester.hpp"
#include "generator/srs/CancellationGenerator.hpp"

namespace benchmark::scenario
{
uint16_t CancellationIncreasingNumOfStates::getNumOfIterationsIn1Dim()
{
    return numOfIterationsIn1Dim;
}

uint16_t CancellationIncreasingNumOfStates::getDim1Details()
{
    return parameters.minNumOfStates * 2; // Cancellation generator increases numOfStates
}

void CancellationIncreasingNumOfStates::runSingleIteration()
{
    Tester{
        numOfTestsInSingleIteration,
        std::unique_ptr<generator::Generator>(new generator::srs::CancellationGenerator()),
        parameters}
        .run();
}
} // namespace benchmark::scenario