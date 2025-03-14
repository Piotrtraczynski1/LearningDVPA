#pragma once

#include <cstdint>
#include <memory>

// tmp:
#include <chrono>
#include <optional>

#include "generator/Generator.hpp"

namespace generator
{

enum class RunResultKind
{
    Ok,
    TimeOut,
    ChildError
};

struct RunResult
{
    RunResultKind kind;
    std::shared_ptr<common::VPA> hyp;
    std::chrono::milliseconds elapsed{};
};

class Tester
{
    std::shared_ptr<common::VPA> vpa;

    // shouldn't be member, VPA should hold pointer to transiton to be changed.
    std::unique_ptr<generator::Generator> generator;

public:
    Tester(const uint64_t numOfTests);

    void run();

private:
    std::string directoryPath;
    uint64_t numOfTests;

    void setOutputDirectoryPath();
    void prepareTest();
    void printTestDetails();
    RunResult runTestWithTimeout(std::chrono::milliseconds timeout);
    void testOutput(const std::shared_ptr<common::VPA> hypothesis);
    void printTestStats();
    void saveFailedTestData(
        const std::shared_ptr<common::Word> testWord,
        const std::shared_ptr<common::VPA> hypothesis);
    void saveFailedTestData(std::string errMsg);
    std::shared_ptr<common::Word> generateRandomWord(uint16_t length);

    uint64_t numOfExecutedTests{0};
    uint64_t numOfFailedTests{0};

    uint64_t numOfHangouts{0};
    uint64_t numOfChildErrors{0};

    uint16_t numOfStates;
    uint16_t numOfCalls;
    uint16_t numOfLocals;
    uint16_t numOfReturns;
    uint16_t numOfStackSymbols;

    std::shared_ptr<common::Word> word;
};
} // namespace generator