#pragma once

#include <array>

#include "TesterParameters.hpp"
#include "benchmark/scenario/Scenario.hpp"

namespace benchmark::scenario
{
enum class MixedSrsGenerator : uint8_t
{
    Cancellation,
    Commutation,
    Idempotency
};

// Replays a fixed subset of the target configurations committed in
// results/data/exactEq.  Every run has SRS enabled and uses random checking
// only inside SrsChecker; the learner's equivalence queries stay exact.
class MixedExactSubsetScenario : public Scenario
{
    struct Step
    {
        uint16_t numOfStates;
        uint16_t secondDvpaNumOfStates;
        uint16_t targetNumOfStates;
        uint16_t numOfStackSymbols;
    };

    static constexpr std::array<Step, 6> cancellationSteps{Step{4, 2, 8, 5},   Step{8, 2, 16, 5},
                                                           Step{10, 2, 20, 5}, Step{12, 2, 24, 5},
                                                           Step{14, 2, 28, 5}, Step{17, 2, 34, 5}};
    static constexpr std::array<Step, 6> commutationSteps{Step{4, 2, 8, 3},  Step{4, 4, 16, 3},
                                                          Step{5, 5, 25, 3}, Step{6, 6, 36, 3},
                                                          Step{7, 6, 42, 3}, Step{7, 7, 49, 3}};
    static constexpr std::array<Step, 6> idempotencySteps{Step{6, 2, 6, 16},   Step{10, 2, 10, 16},
                                                          Step{14, 2, 14, 16}, Step{18, 2, 18, 16},
                                                          Step{22, 2, 22, 16}, Step{26, 2, 26, 16}};

    static constexpr int firstSeed{52};
    static constexpr uint16_t numOfSeeds{50};

    const MixedSrsGenerator generator;
    uint16_t stepIndex{0};
    TesterParameters parameters;

public:
    explicit MixedExactSubsetScenario(MixedSrsGenerator generatorArg);

    int getSeed() override;
    uint16_t getNumOfIterationsIn1Dim() override;
    uint16_t getNumOfTestsInSingleIteration() override;
    std::string getDim1Name() override;
    uint16_t getDim1Details() override;
    void prepareNextIterationDim1() override;
    void resetDim1() override;
    const TesterParameters &getParameters() const override;
    std::string getGeneratorName() const override;
    SingleTestResult runSingle(uint32_t seed) override;

private:
    const Step &currentStep() const;
    void applyStep();
};
} // namespace benchmark::scenario
