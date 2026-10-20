#include "benchmark/scenario/srs/ProbabilisticSrsReplayScenario.hpp"

#include <cstdlib>
#include <memory>

#include "Tester.hpp"
#include "generator/srs/CancellationGenerator.hpp"
#include "generator/srs/CommutativeGenerator.hpp"
#include "generator/srs/IdempotencyGenerator.hpp"
#include "teacher/EquivalenceCheck.hpp"
#include "utils/log.hpp"

namespace benchmark::scenario
{
ProbabilisticSrsReplayScenario::ProbabilisticSrsReplayScenario()
    : generator{ReplayGenerator::Cancellation},
      parameters{TesterParameters{
          .minNumOfStates = 5,
          .maxNumOfStates = 12,
          .minNumOfCalls = 2,
          .maxNumOfCalls = 5,
          .minNumOfLocals = 2,
          .maxNumOfLocals = 5,
          .minNumOfReturns = 2,
          .maxNumOfReturns = 5,
          .minNumOfStackSymbols = 2,
          .maxNumOfStackSymbols = 5,
          .numOfRandomTestingWords = 0,
          .maxTestingWordLength = 200,
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
          .supervisedTestMaxDuration = std::chrono::seconds{600}}}
{
#ifndef RANDOM_EQUIVALENCE_QUERY
    ERR("This scenario requires probabilistic EQ. Recompile with -randomEQ");
    std::exit(1);
#endif
    configureGenerator();
}

void ProbabilisticSrsReplayScenario::configureGenerator()
{
    parameters.srsCheckConfig = {.method = utils::SrsCheckMethod::Mixed};
    // Reset all ranges so idempotency cannot inherit commutation settings.
    {
        parameters.minNumOfStates = 5;
        parameters.maxNumOfStates = 12;
        parameters.minNumOfCalls = 2;
        parameters.maxNumOfCalls = 5;
        parameters.minNumOfLocals = 2;
        parameters.maxNumOfLocals = 5;
        parameters.minNumOfReturns = 2;
        parameters.maxNumOfReturns = 5;
        parameters.minNumOfStackSymbols = 2;
        parameters.maxNumOfStackSymbols = 5;
        parameters.minSecondDvpaNumOfStates = 2;
        parameters.maxSecondDvpaNumOfStates = 2;
    }
    if (generator == ReplayGenerator::Commutation)
    {
        parameters.minNumOfStates = 2;
        parameters.maxNumOfStates = 6;
        parameters.minNumOfStackSymbols = 2;
        parameters.maxNumOfStackSymbols = 2;
        parameters.minSecondDvpaNumOfStates = 2;
        parameters.maxSecondDvpaNumOfStates = 5;
    }
    else if (generator == ReplayGenerator::Idempotency)
    {
        parameters.minNumOfStates = 10;
        parameters.maxNumOfStates = 24;
    }
}

int ProbabilisticSrsReplayScenario::getSeed()
{
    return generator == ReplayGenerator::Idempotency ? 1 : 601;
}
uint16_t ProbabilisticSrsReplayScenario::getNumOfIterationsIn1Dim()
{
    return 3;
}
std::string ProbabilisticSrsReplayScenario::getDim1Name()
{
    return "generator";
}
uint16_t ProbabilisticSrsReplayScenario::getDim1Details()
{
    return static_cast<uint16_t>(generator);
}
void ProbabilisticSrsReplayScenario::prepareNextIterationDim1()
{
    if (generator != ReplayGenerator::Idempotency)
    {
#ifdef RANDOM_EQUIVALENCE_QUERY
        teacher::resetRandomEquivalenceCheckGenerator();
#endif
        generator = static_cast<ReplayGenerator>(static_cast<uint8_t>(generator) + 1);
        configureGenerator();
    }
}
uint16_t ProbabilisticSrsReplayScenario::getNumOfTestsInSingleIteration()
{
    return generator == ReplayGenerator::Idempotency ? 6000 : 5400;
}
const TesterParameters &ProbabilisticSrsReplayScenario::getParameters() const
{
    return parameters;
}

std::string ProbabilisticSrsReplayScenario::getGeneratorName() const
{
    switch (generator)
    {
    case ReplayGenerator::Cancellation:
        return "cancellation";
    case ReplayGenerator::Commutation:
        return "commutative";
    case ReplayGenerator::Idempotency:
        return "idempotency";
    }
    return "unknown";
}

SingleTestResult ProbabilisticSrsReplayScenario::runSingle(const uint32_t seed)
{
    switch (generator)
    {
    case ReplayGenerator::Cancellation:
        return Tester{
            1, std::make_unique<generator::srs::CancellationGenerator>(), parameters, seed}
            .runSingle();
    case ReplayGenerator::Commutation:
        return Tester{1, std::make_unique<generator::srs::CommutativeGenerator>(), parameters, seed}
            .runSingle();
    case ReplayGenerator::Idempotency:
        return Tester{1, std::make_unique<generator::srs::IdempotencyGenerator>(), parameters, seed}
            .runSingle();
    }
    std::abort();
}
} // namespace benchmark::scenario
