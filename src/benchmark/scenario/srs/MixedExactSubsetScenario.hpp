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

// Collects the missing cases relative to main's mixed-exact-subset CSVs.
// Existing configurations continue at seed 102; new/corrected ones start at 52.
// Only SRS checks are random; the learner's equivalence queries stay exact.
class MixedExactSubsetScenario : public Scenario
{
    struct Step
    {
        uint16_t numOfStates;
        uint16_t secondDvpaNumOfStates;
        uint16_t targetNumOfStates;
        uint16_t numOfStackSymbols;
        int firstSeed;
        uint16_t numOfSeeds;
    };

    static constexpr std::array<Step, 8> cancellationSteps{
        Step{2, 2, 4, 5, 52, 100},   Step{4, 2, 8, 5, 102, 50},   Step{6, 2, 12, 5, 52, 100},
        Step{8, 2, 16, 5, 102, 50},  Step{10, 2, 20, 5, 102, 50}, Step{12, 2, 24, 5, 102, 50},
        Step{14, 2, 28, 5, 102, 50}, Step{17, 2, 34, 5, 102, 50}};
    static constexpr std::array<Step, 13> commutationSteps{
        Step{2, 2, 4, 3, 52, 100}, Step{2, 3, 6, 3, 52, 100},
        // Correct the old (4, 2) configuration to the baseline's (2, 4).
        Step{2, 4, 8, 3, 52, 100}, Step{3, 3, 9, 3, 52, 100}, Step{2, 5, 10, 3, 52, 100},
        Step{3, 4, 12, 3, 52, 100}, Step{4, 4, 16, 3, 102, 50}, Step{4, 5, 20, 3, 52, 100},
        Step{5, 5, 25, 3, 102, 50}, Step{6, 5, 30, 3, 52, 100}, Step{6, 6, 36, 3, 102, 50},
        Step{7, 6, 42, 3, 102, 50}, Step{7, 7, 49, 3, 102, 50}};
    static constexpr std::array<Step, 7> idempotencySteps{
        Step{5, 2, 5, 16, 52, 100},   Step{6, 2, 6, 16, 102, 50},   Step{10, 2, 10, 16, 102, 50},
        Step{14, 2, 14, 16, 102, 50}, Step{18, 2, 18, 16, 102, 50}, Step{22, 2, 22, 16, 102, 50},
        Step{26, 2, 26, 16, 102, 50}};

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
