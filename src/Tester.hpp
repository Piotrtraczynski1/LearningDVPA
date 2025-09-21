#pragma once

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "TesterParameters.hpp"
#include "generator/Generator.hpp"
#include "learner/Learner.hpp"
#include "teacher/Teacher.hpp"

struct RunResult;

class Tester
{
    const TesterParameters &params;

    std::shared_ptr<common::VPA> vpa;
    std::unique_ptr<generator::Generator> generator;

public:
    Tester(
        const uint64_t numOfTests, std::unique_ptr<generator::Generator> generator,
        const TesterParameters &testerParameters);

    void run();

private:
    std::string directoryPath;
    uint64_t numOfTests;
    std::ofstream outputFile;

    void setOutputDirectoryPath();
    void setOutputFile();
    void printTestDetails(std::ostream &os = std::cout);
    void printAutomaton(const std::shared_ptr<common::VPA> automaton, const std::string &name);
    void printUt();

    void prepareTest();
    std::shared_ptr<common::VPA> runLearner();
    void runSupervisedTest();
    RunResult runTestWithTimeout();

    std::shared_ptr<common::Word> generateRandomWord(uint16_t length);
    void testOutput(const std::shared_ptr<common::VPA> hypothesis);
    void printTestStats();
    void saveFailedTestData(
        const std::shared_ptr<common::Word> testWord,
        const std::shared_ptr<common::VPA> hypothesis);
    void saveFailedTestData(std::string errMsg);
    void saveTestData(const std::shared_ptr<common::VPA> hypothesis, const std::string &msg);

    uint64_t numOfExecutedTests{0};
    uint64_t numOfFailedTests{0};

    uint64_t numOfHangouts{0};
    uint64_t numOfChildErrors{0};
    uint64_t numOfErrInHandleStackContent{0};
    uint64_t numOfGeneratorCheckFailed{0};

    uint16_t numOfStates;
    uint16_t numOfCalls;
    uint16_t numOfLocals;
    uint16_t numOfReturns;
    uint16_t numOfStackSymbols;

    std::shared_ptr<teacher::Converter> converter;
    std::shared_ptr<teacher::Teacher> teacher;
    std::shared_ptr<learner::Learner> learner;
};

enum class RunResultKind
{
    Ok,
    Timeout,
    ChildError,
    HandleStackContentDiverages
};

struct RunResult
{
    RunResultKind kind;
    std::shared_ptr<common::VPA> hyp;
    std::chrono::milliseconds elapsed{};
};