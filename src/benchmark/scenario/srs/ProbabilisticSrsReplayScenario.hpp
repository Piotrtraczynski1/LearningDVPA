#pragma once

#include "TesterParameters.hpp"
#include "benchmark/scenario/Scenario.hpp"

namespace benchmark::scenario
{
enum class ReplayGenerator : uint8_t
{
    Cancellation,
    Commutation,
    Idempotency
};

// Replays the three probabilisticEq accuracy generators with tuned SRS
// constants and the current mixed checker. Cancellation/commutation continue
// at seed 601; corrected idempotency starts again at seed 1.
class ProbabilisticSrsReplayScenario : public Scenario
{
    ReplayGenerator generator;
    TesterParameters parameters;
    void configureGenerator();

public:
    ProbabilisticSrsReplayScenario();
    int getSeed() override;
    uint16_t getNumOfIterationsIn1Dim() override;
    std::string getDim1Name() override;
    uint16_t getDim1Details() override;
    void prepareNextIterationDim1() override;
    uint16_t getNumOfTestsInSingleIteration() override;
    const TesterParameters &getParameters() const override;
    std::string getGeneratorName() const override;
    SingleTestResult runSingle(uint32_t seed) override;
};
} // namespace benchmark::scenario
