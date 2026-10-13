#include "benchmark/scenario/srs/MixedExactSubsetScenario.hpp"

#include <cstdlib>
#include <memory>

#include "Tester.hpp"
#include "generator/srs/CancellationGenerator.hpp"
#include "generator/srs/CommutativeGenerator.hpp"
#include "generator/srs/IdempotencyGenerator.hpp"
#include "utils/log.hpp"

namespace benchmark::scenario
{
namespace
{
TesterParameters makeParameters()
{
    return TesterParameters{
        .minNumOfStates = 2,
        .maxNumOfStates = 2,
        .minNumOfCalls = 3,
        .maxNumOfCalls = 3,
        .minNumOfLocals = 3,
        .maxNumOfLocals = 3,
        .minNumOfReturns = 3,
        .maxNumOfReturns = 3,
        .minNumOfStackSymbols = 5,
        .maxNumOfStackSymbols = 5,
        .numOfRandomTestingWords = 0,
        .maxTestingWordLength = 30,
        .density = 0.8,
        .acceptingStatesDensity = 0.3,
        .numOfModules = 5,
        .useSrs = true,
        .srsCheckConfig = {.method = utils::SrsCheckMethod::Mixed},
        .minSecondDvpaNumOfStates = 2,
        .maxSecondDvpaNumOfStates = 2,
        .useEquivalenceCheckToValidateOutput = true,
        .savePassedTestData = false,
        .supervisedMode = false,
        .supervisedTestMaxDuration = std::chrono::seconds{600}};
}

const char *generatorName(const MixedSrsGenerator generator)
{
    switch (generator)
    {
    case MixedSrsGenerator::Cancellation:
        return "cancellation";
    case MixedSrsGenerator::Commutation:
        return "commutative";
    case MixedSrsGenerator::Idempotency:
        return "idempotency";
    }
    return "unknown";
}
} // namespace

MixedExactSubsetScenario::MixedExactSubsetScenario(const MixedSrsGenerator generatorArg)
    : generator{generatorArg}, parameters{makeParameters()}
{
#ifndef MIXED_EQUIVALENCE_QUERY
    ERR("This scenario requires mixed SRS checks. Recompile with -mixedEQ");
    std::exit(1);
#endif
    applyStep();
}

int MixedExactSubsetScenario::getSeed()
{
    return firstSeed;
}

uint16_t MixedExactSubsetScenario::getNumOfIterationsIn1Dim()
{
    return cancellationSteps.size();
}

uint16_t MixedExactSubsetScenario::getNumOfTestsInSingleIteration()
{
    return numOfSeeds;
}

std::string MixedExactSubsetScenario::getDim1Name()
{
    return "exactEqSubsetTargetNumOfStates";
}

uint16_t MixedExactSubsetScenario::getDim1Details()
{
    return currentStep().targetNumOfStates;
}

void MixedExactSubsetScenario::prepareNextIterationDim1()
{
    if (stepIndex + 1 < cancellationSteps.size())
    {
        stepIndex++;
        applyStep();
    }
}

void MixedExactSubsetScenario::resetDim1()
{
    stepIndex = 0;
    applyStep();
}

const TesterParameters &MixedExactSubsetScenario::getParameters() const
{
    return parameters;
}

std::string MixedExactSubsetScenario::getGeneratorName() const
{
    return generatorName(generator);
}

SingleTestResult MixedExactSubsetScenario::runSingle(const uint32_t seed)
{
    SingleTestResult result;
    switch (generator)
    {
    case MixedSrsGenerator::Cancellation:
        result = Tester{1, std::make_unique<generator::srs::CancellationGenerator>(), parameters, seed}
                     .runSingle();
        break;
    case MixedSrsGenerator::Commutation:
        result = Tester{1, std::make_unique<generator::srs::CommutativeGenerator>(), parameters, seed}
                     .runSingle();
        break;
    case MixedSrsGenerator::Idempotency:
        result = Tester{1, std::make_unique<generator::srs::IdempotencyGenerator>(), parameters, seed}
                     .runSingle();
        break;
    }

    if (result.targetNumOfStates != currentStep().targetNumOfStates)
    {
        ERR("Unexpected target size: expected %u, got %u", currentStep().targetNumOfStates,
            result.targetNumOfStates);
        std::exit(1);
    }
    return result;
}

const MixedExactSubsetScenario::Step &MixedExactSubsetScenario::currentStep() const
{
    switch (generator)
    {
    case MixedSrsGenerator::Cancellation:
        return cancellationSteps[stepIndex];
    case MixedSrsGenerator::Commutation:
        return commutationSteps[stepIndex];
    case MixedSrsGenerator::Idempotency:
        return idempotencySteps[stepIndex];
    }
    return cancellationSteps[stepIndex];
}

void MixedExactSubsetScenario::applyStep()
{
    const auto &step{currentStep()};
    parameters.minNumOfStates = step.numOfStates;
    parameters.maxNumOfStates = step.numOfStates;
    parameters.minSecondDvpaNumOfStates = step.secondDvpaNumOfStates;
    parameters.maxSecondDvpaNumOfStates = step.secondDvpaNumOfStates;
    parameters.minNumOfStackSymbols = step.numOfStackSymbols;
    parameters.maxNumOfStackSymbols = step.numOfStackSymbols;
}
} // namespace benchmark::scenario
