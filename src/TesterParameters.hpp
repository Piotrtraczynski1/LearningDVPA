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

    uint16_t numOfRandomTestingWords;
    uint16_t maxTestingWordLength;

    double density;
    double acceptingStatesDensity{0.3};

    uint16_t numOfModules{0};

    bool useSrs{false};

    bool savePassedTestData;
    bool supervisedMode;
    std::chrono::seconds supervisedTestMaxDuration;
};

constexpr TesterParameters RandomTestParameters{
    .minNumOfStates = 16,
    .maxNumOfStates = 16,
    .minNumOfCalls = 3,
    .maxNumOfCalls = 3,
    .minNumOfLocals = 3,
    .maxNumOfLocals = 3,
    .minNumOfReturns = 3,
    .maxNumOfReturns = 3,
    .minNumOfStackSymbols = 5,
    .maxNumOfStackSymbols = 5,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

// Assumption: numOfCalls = numOfReturns = NumOfStackSymbol - 1
constexpr TesterParameters XMLTestParameters{
    .minNumOfStates = 30,
    .maxNumOfStates = 30,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 7,
    .maxNumOfLocals = 7,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 6,
    .maxNumOfStackSymbols = 6,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};

// if useSrs = true, then numOfStackSymbols should be equal numOfModules + 1
constexpr TesterParameters eCDATestParameters{
    .minNumOfStates = 10,
    .maxNumOfStates = 10,
    .minNumOfCalls = 5,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 15,
    .maxNumOfLocals = 15,
    .minNumOfReturns = 5,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 4,
    .maxNumOfStackSymbols = 4,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 1,
    .numOfModules = 3,
    .useSrs = true,
    .savePassedTestData = false,
    .supervisedMode = false,
    .supervisedTestMaxDuration = std::chrono::seconds{600}};
