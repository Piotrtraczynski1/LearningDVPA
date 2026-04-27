#include "benchmark/scenario/srs/CommutationIncreasingNumOfStates.hpp"
#include "Tester.hpp"
#include "generator/srs/CommutativeGenerator.hpp"

namespace benchmark::scenario
{
using Step = std::pair<uint16_t, uint16_t>;

const std::array<Step, CommutationIncreasingNumOfStates::numOfIterationsIn1Dim>
    CommutationIncreasingNumOfStates::dim1Steps = {Step{4, 5}};

uint16_t CommutationIncreasingNumOfStates::getNumOfIterationsIn1Dim()
{
    return numOfIterationsIn1Dim;
}

uint16_t CommutationIncreasingNumOfStates::getDim1Details()
{
    return (parameters.minNumOfStates + 1) *
           (parameters.secondDvpaNumOfStates + 1); // Commutative generator increases numOfStates
}

void CommutationIncreasingNumOfStates::runSingleIteration()
{
    Tester{
        numOfTestsInSingleIteration,
        std::unique_ptr<generator::Generator>(new generator::srs::CommutativeGenerator()),
        parameters, rand()}
        .run();
}

void CommutationIncreasingNumOfStates::prepareNextIterationDim1()
{
    dim1Step++;
    if (dim1Step == numOfIterationsIn1Dim)
        return;
    parameters.minNumOfStates = dim1Steps[dim1Step].first;
    parameters.maxNumOfStates = dim1Steps[dim1Step].first;
    parameters.secondDvpaNumOfStates = dim1Steps[dim1Step].second;
}

void CommutationIncreasingNumOfStates::resetDim1()
{
    dim1Step = 0;
    parameters.minNumOfStates = dim1Steps[dim1Step].first;
    parameters.maxNumOfStates = dim1Steps[dim1Step].first;
    parameters.secondDvpaNumOfStates = dim1Steps[dim1Step].second;
}
} // namespace benchmark::scenario