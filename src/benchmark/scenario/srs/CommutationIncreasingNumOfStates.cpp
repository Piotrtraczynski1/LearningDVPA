#include "benchmark/scenario/srs/CommutationIncreasingNumOfStates.hpp"
#include "Tester.hpp"
#include "generator/srs/CommutativeGenerator.hpp"

namespace benchmark::scenario
{
using Step = std::pair<uint16_t, uint16_t>;

const std::array<Step, CommutationIncreasingNumOfStates::numOfIterationsIn1Dim>
    CommutationIncreasingNumOfStates::dim1Steps = {Step{2, 2}, Step{2, 3}, Step{2, 4}, Step{3, 3},
                                                   Step{2, 5}, Step{3, 4}, Step{4, 4}, Step{4, 5},
                                                   Step{5, 5}, Step{6, 5}, Step{6, 6}, Step{7, 6}};

uint16_t CommutationIncreasingNumOfStates::getNumOfIterationsIn1Dim()
{
    return numOfIterationsIn1Dim;
}

uint16_t CommutationIncreasingNumOfStates::getDim1Details()
{
    return (parameters.minNumOfStates + 1) *
           (parameters.minSecondDvpaNumOfStates + 1); // Commutative generator increases numOfStates
}

std::string CommutationIncreasingNumOfStates::getGeneratorName() const
{
    return "commutative";
}

SingleTestResult CommutationIncreasingNumOfStates::runSingle(uint32_t seed)
{
    return Tester{1, std::make_unique<generator::srs::CommutativeGenerator>(), parameters, seed}
        .runSingle();
}

void CommutationIncreasingNumOfStates::prepareNextIterationDim1()
{
    dim1Step++;
    if (dim1Step == numOfIterationsIn1Dim)
    {
        return;
    }
    parameters.minNumOfStates = dim1Steps[dim1Step].first;
    parameters.maxNumOfStates = dim1Steps[dim1Step].first;
    parameters.minSecondDvpaNumOfStates = dim1Steps[dim1Step].second;
    parameters.maxSecondDvpaNumOfStates = dim1Steps[dim1Step].second;
}

void CommutationIncreasingNumOfStates::resetDim1()
{
    dim1Step = 0;
    parameters.minNumOfStates = dim1Steps[dim1Step].first;
    parameters.maxNumOfStates = dim1Steps[dim1Step].first;
    parameters.minSecondDvpaNumOfStates = dim1Steps[dim1Step].second;
    parameters.maxSecondDvpaNumOfStates = dim1Steps[dim1Step].second;
}
} // namespace benchmark::scenario
