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

    bool savePassedTestData;
    bool supervisedMode;
    std::chrono::seconds supervisedTestMaxDuration;
};

constexpr TesterParameters RandomTestParameters{
    .minNumOfStates = 4,
    .maxNumOfStates = 10,
    .minNumOfCalls = 2,
    .maxNumOfCalls = 5,
    .minNumOfLocals = 2,
    .maxNumOfLocals = 5,
    .minNumOfReturns = 2,
    .maxNumOfReturns = 5,
    .minNumOfStackSymbols = 2,
    .maxNumOfStackSymbols = 8,
    .numOfRandomTestingWords = 10000,
    .maxTestingWordLength = 30,
    .density = 0.2,
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