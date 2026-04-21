#pragma once

#include <chrono>
#include <cstdint>

struct TesterParameters
{
    uint16_t minNumOfStates;
    uint16_t maxNumOfStates;

    uint16_t minNumOfCalls;
    uint16_t maxNumOfCalls;

    uint16_t minNumOfLocals;
    uint16_t maxNumOfLocals;

    uint16_t minNumOfReturns;
    uint16_t maxNumOfReturns;

    uint16_t minNumOfStackSymbols;
    uint16_t maxNumOfStackSymbols;

    uint32_t numOfRandomTestingWords;
    uint16_t maxTestingWordLength;

    double density;
    double acceptingStatesDensity{0.4};

    uint16_t numOfModules{0};

    bool useSrs{false};
    uint16_t secondDvpaNumOfStates{3};

    bool useEquivalenceCheckToValidateOutput{false};
    bool savePassedTestData;
    bool supervisedMode;
    std::chrono::seconds supervisedTestMaxDuration;
};

#ifndef RANDOM_EQUIVALENCE_QUERY

constexpr TesterParameters RandomTestParameters{
    .minNumOfStates = 5,
    .maxNumOfStates = 5,
    .minNumOfCalls = 2,
    .maxNumOfCalls = 2,
    .minNumOfLocals = 3,
    .maxNumOfLocals = 3,
    .minNumOfReturns = 3,
    .maxNumOfReturns = 3,
    .minNumOfStackSymbols = 4,
    .maxNumOfStackSymbols = 4,
    .numOfRandomTestingWords = 65000,
    .maxTestingWordLength = 10,
    .density = 0.9,
    .useEquivalenceCheckToValidateOutput = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters CommutativeTestParameters{
    .minNumOfStates = 5,
    .maxNumOfStates = 2,
    .minNumOfCalls = 8,
    .maxNumOfCalls = 8,
    .minNumOfLocals = 2,
    .maxNumOfLocals = 2,
    .minNumOfReturns = 8,
    .maxNumOfReturns = 8,
    .minNumOfStackSymbols = 2,
    .maxNumOfStackSymbols = 2,
    .numOfRandomTestingWords = 65000,
    .maxTestingWordLength = 10,
    .density = 0.9,
    .useSrs = true,
    .secondDvpaNumOfStates = 3,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters CancellationTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 3,
    .maxNumOfCalls = 7,
    .minNumOfLocals = 2,
    .maxNumOfLocals = 10,
    .minNumOfReturns = 4,
    .maxNumOfReturns = 7,
    .minNumOfStackSymbols = 2,
    .maxNumOfStackSymbols = 4,
    .numOfRandomTestingWords = 65000,
    .maxTestingWordLength = 10,
    .density = 0.9,
    .useSrs = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters IdempotencyTestParameters{
    .minNumOfStates = 5,
    .maxNumOfStates = 10,
    .minNumOfCalls = 4,
    .maxNumOfCalls = 4,
    .minNumOfLocals = 5,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 4,
    .maxNumOfReturns = 4,
    .minNumOfStackSymbols = 16,
    .maxNumOfStackSymbols = 16,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 5,
    .useSrs = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters CdaTestParameters{
    .minNumOfStates = 3,
    .maxNumOfStates = 8,
    .minNumOfCalls = 4,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 15,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 3,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 16,
    .maxNumOfStackSymbols = 16,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 3,
    .useSrs = false,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters SeVpaTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 5,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 16,
    .maxNumOfStackSymbols = 16,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 3,
    .useSrs = false,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters MeVpaTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 5,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 4,
    .maxNumOfStackSymbols = 4,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 3,
    .useSrs = false,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters ECdaTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 4,
    .maxNumOfCalls = 4,
    .minNumOfLocals = 2,
    .maxNumOfLocals = 10,
    .minNumOfReturns = 1,
    .maxNumOfReturns = 7,
    .minNumOfStackSymbols = 6,
    .maxNumOfStackSymbols = 6,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 5,
    .useSrs = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

#else // RANDOM_EQUIVALENCE_QUERY

constexpr TesterParameters RandomTestParameters{
    .minNumOfStates = 300,
    .maxNumOfStates = 400,
    .minNumOfCalls = 10,
    .maxNumOfCalls = 20,
    .minNumOfLocals = 10,
    .maxNumOfLocals = 20,
    .minNumOfReturns = 10,
    .maxNumOfReturns = 20,
    .minNumOfStackSymbols = 10,
    .maxNumOfStackSymbols = 20,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 0.8,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters CommutativeTestParameters{
    .minNumOfStates = 7,
    .maxNumOfStates = 8,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 15,
    .minNumOfLocals = 5,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 15,
    .minNumOfStackSymbols = 4,
    .maxNumOfStackSymbols = 10,
    .numOfRandomTestingWords = 500000,
    .maxTestingWordLength = 200,
    .density = 0.9,
    .useSrs = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters CancellationTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 11,
    .minNumOfCalls = 2,
    .maxNumOfCalls = 3,
    .minNumOfLocals = 6,
    .maxNumOfLocals = 7,
    .minNumOfReturns = 6,
    .maxNumOfReturns = 7,
    .minNumOfStackSymbols = 7,
    .maxNumOfStackSymbols = 10,
    .numOfRandomTestingWords = 500000,
    .maxTestingWordLength = 200,
    .density = 0.9,
    .useSrs = true,
    .useEquivalenceCheckToValidateOutput = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters IdempotencyTestParameters{
    .minNumOfStates = 50,
    .maxNumOfStates = 60,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 15,
    .minNumOfLocals = 5,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 15,
    .minNumOfStackSymbols = 10,
    .maxNumOfStackSymbols = 20,
    .numOfRandomTestingWords = 500000,
    .maxTestingWordLength = 200,
    .density = 0.9,
    .useSrs = false,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters CdaTestParameters{
    .minNumOfStates = 3,
    .maxNumOfStates = 8,
    .minNumOfCalls = 4,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 15,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 3,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 16,
    .maxNumOfStackSymbols = 16,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 3,
    .useSrs = false,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters SeVpaTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 5,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 16,
    .maxNumOfStackSymbols = 16,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 3,
    .useSrs = false,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters MeVpaTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 5,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 4,
    .maxNumOfStackSymbols = 4,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 3,
    .useSrs = false,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

constexpr TesterParameters ECdaTestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 4,
    .maxNumOfCalls = 4,
    .minNumOfLocals = 2,
    .maxNumOfLocals = 10,
    .minNumOfReturns = 1,
    .maxNumOfReturns = 7,
    .minNumOfStackSymbols = 6,
    .maxNumOfStackSymbols = 6,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 5,
    .useSrs = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

#endif // not RANDOM_EQUIVALENCE_QUERY