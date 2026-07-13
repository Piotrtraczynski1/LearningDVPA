#include <cerrno>
#include <cstdlib>
#include <memory>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "Tester.hpp"
#include "learner/srs/Srs.hpp"
#include "teacher/AutomataCombiner.hpp"
#include "teacher/EmptinessChecker.hpp"
#include "utils/TimeMarker.hpp"

Tester::Tester(
    const uint64_t numOfTests, std::unique_ptr<generator::Generator> gen,
    const TesterParameters &testerParameters, uint32_t seed)
    : params{testerParameters}, generator{std::move(gen)}, numOfTests{numOfTests}, seed{seed}
{
    generator->setSeed(seed + 1);
    rng = std::mt19937{seed};
    setOutputDirectoryPath();
}

void Tester::setOutputDirectoryPath()
{
    std::string path = std::string(__FILE__);
    path = path.substr(0, path.find_last_of("/\\") + 1);
    directoryPath = path + "failedTests/";
}

uint16_t Tester::run()
{
    while (numOfExecutedTests < numOfTests)
    {
        runNextTest();
    }

    return numOfFailedTests;
}

const char *toString(TestStatus status)
{
    switch (status)
    {
    case TestStatus::Passed:
        return "passed";
    case TestStatus::ValidationFailed:
        return "validation_failed";
    case TestStatus::GeneratorCheckFailed:
        return "generator_check_failed";
    case TestStatus::Timeout:
        return "timeout";
    case TestStatus::ChildError:
        return "child_error";
    case TestStatus::StackContentError:
        return "stack_content_error";
    }

    return "child_error";
}

namespace
{
uint16_t countAcceptingStates(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &automaton)
{
    if (not automaton)
    {
        return 0;
    }

    uint16_t count{0};
    for (bool accepting : automaton->acceptingStates)
    {
        count += accepting;
    }
    return count;
}
} // namespace

SingleTestResult Tester::runSingle()
{
    if (numOfTests != 1 or numOfExecutedTests != 0)
    {
        ERR("[Tester]: runSingle requires a fresh Tester configured for one test");
        exit(toExit(ExitCode::UNDEFINED));
    }

    return runNextTest();
}

SingleTestResult Tester::runNextTest()
{
    prepareTest();
    printTestDetails();
    currentTestStatus = TestStatus::Passed;

    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis;
    if (params.supervisedMode)
    {
        hypothesis = runSupervisedTest();
    }
    else
    {
        hypothesis = runLearner();
        testOutput(hypothesis);
    }

    numOfExecutedTests++;
    printTestStats();

    return SingleTestResult{
        .seed = seed,
        .numOfStates = numOfStates,
        .numOfCalls = numOfCalls,
        .numOfLocals = numOfLocals,
        .numOfReturns = numOfReturns,
        .numOfStackSymbols = numOfStackSymbols,
        .targetNumOfStates = vpa->getNumOfStates(),
        .targetAcceptingStates = countAcceptingStates(vpa),
        .hypothesisNumOfStates = hypothesis ? hypothesis->getNumOfStates() : uint16_t{0},
        .hypothesisAcceptingStates = countAcceptingStates(hypothesis),
        .status = currentTestStatus};
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> Tester::runLearner()
{
    teacher = std::make_shared<teacher::Teacher>(
        vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
    learner::srs::Srs srs = params.useSrs ? generator->getSrs() : learner::srs::Srs{};
    learner = std::make_shared<learner::Learner>(
        *teacher, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols, srs);

    return learner->run();
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> Tester::runSupervisedTest()
{
    auto res = runTestWithTimeout();
    switch (res.exitCode)
    {
    case ExitCode::OK:
        testOutput(res.hyp);
        return res.hyp;
    case ExitCode::TIMEOUT:
        ERR("[Tester]: TIMEOUT after %ld ms", res.elapsed.count());
        numOfHangouts++;
        currentTestStatus = TestStatus::Timeout;
        saveFailedTestData("HANGOUT DETECTED!");
        return nullptr;
    case ExitCode::STACKCONTENTDIVERGE:
        ERR("[Tester]: ERROR in StackContentDiverages, after %ld ms", res.elapsed.count());
        numOfErrInHandleStackContent++;
        currentTestStatus = TestStatus::StackContentError;
        saveFailedTestData("StackContentDiverages ERROR!");
        return nullptr;
    default:
        ERR("[Tester]: Child exited early, after %ld ms with error: %s", res.elapsed.count(),
            toString(res.exitCode));
        numOfChildErrors++;
        currentTestStatus = TestStatus::ChildError;
        saveFailedTestData("CHILD ERROR: " + std::string(toString(res.exitCode)));
        return nullptr;
    }

    return nullptr;
}

void Tester::prepareTest()
{
    numOfStates =
        (rng() % (params.maxNumOfStates - params.minNumOfStates + 1)) + params.minNumOfStates;
    numOfCalls = (rng() % (params.maxNumOfCalls - params.minNumOfCalls + 1)) + params.minNumOfCalls;
    numOfLocals =
        (rng() % (params.maxNumOfLocals - params.minNumOfLocals + 1)) + params.minNumOfLocals;
    numOfReturns =
        (rng() % (params.maxNumOfReturns - params.minNumOfReturns + 1)) + params.minNumOfReturns;
    numOfStackSymbols = (rng() % (params.maxNumOfStackSymbols - params.minNumOfStackSymbols + 1)) +
                        params.minNumOfStackSymbols;

    generator->setConfig(
        numOfStates, numOfCalls, numOfLocals, numOfReturns, numOfStackSymbols, params.density,
        params.acceptingStatesDensity, params.numOfModules, params.secondDvpaNumOfStates);
    vpa = generator->run();
}

void Tester::printTestDetails(std::ostream &os)
{
    os << "Prepared test number: " << numOfExecutedTests << "\n numOfStates: " << numOfStates
       << ", numOfCalls: " << numOfCalls << ", numOfLocals: " << numOfLocals
       << ", numOfReturns: " << numOfReturns << ", numOfStackSymbols: " << numOfStackSymbols
       << std::endl;
}

RunResult Tester::runTestWithTimeout()
{
    using clock = std::chrono::steady_clock;
    auto startTime = clock::now();

    pid_t pid = fork();

    if (pid == 0)
    {
        (void)runLearner();
        exit(toExit(ExitCode::OK));
    }

    const auto deadline = clock::now() + params.supervisedTestMaxDuration;
    int status = 0;
    while (true)
    {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r == pid)
        {
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - startTime);
            if (WIFEXITED(status))
            {
                const auto exitCode = ExitCode{static_cast<uint8_t>(WEXITSTATUS(status))};
                if (exitCode == ExitCode::OK)
                {
                    return {ExitCode::OK, runLearner(), elapsed};
                }

                return {exitCode, nullptr, elapsed};
            }

            return {ExitCode::UNDEFINED, nullptr, elapsed};
        }

        if (clock::now() >= deadline)
        {
            kill(pid, SIGKILL);
            (void)waitpid(pid, &status, 0);
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - startTime);
            return {ExitCode::TIMEOUT, nullptr, elapsed};
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

std::shared_ptr<common::Word> Tester::generateRandomWord(uint16_t length)
{
    common::Word word{};

    for (uint16_t i = 0; i < length; i++)
    {
        uint8_t symbol{static_cast<uint8_t>(rng() % 3)};

        switch (symbol)
        {
        case 0:
            if (numOfCalls != 0)
            {
                word += common::Word{
                    common::symbol::CallSymbol{static_cast<uint16_t>(rng() % numOfCalls)}};
            }
            break;
        case 1:
            if (numOfLocals != 0)
            {
                word += common::Word{
                    common::symbol::LocalSymbol{static_cast<uint16_t>(rng() % numOfLocals)}};
            }
            break;
        case 2:
            if (numOfReturns != 0)
            {
                word += common::Word{
                    common::symbol::ReturnSymbol{static_cast<uint16_t>(rng() % numOfReturns)}};
            }
            break;
        default:
            ERR("[Tester]: Can not generate random word");
        }
    }

    return std::make_shared<common::Word>(word);
}

void Tester::testOutput(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    TIME_MARKER("[Tester]: testOutput");
    if (params.useEquivalenceCheckToValidateOutput)
    {
        testOutputWithEquivalenceCheck(hypothesis);
        return;
    }

    if (not generator->generatorSpecificCheck(hypothesis))
    {
        ERR("[Tester]: Generator specific check failed!");
        numOfGeneratorCheckFailed++;
        currentTestStatus = TestStatus::GeneratorCheckFailed;
        saveTestData(hypothesis, "GENERATOR SPECIFIC CHECK FAILED");
        return;
    }

    for (uint32_t i = 0; i < params.numOfRandomTestingWords; i++)
    {
        const uint16_t length{static_cast<uint16_t>(rng() % (params.maxTestingWordLength + 1))};
        std::shared_ptr<common::Word> testWord{generateRandomWord(length)};

        if (hypothesis->checkWord(*testWord) != vpa->checkWord(*testWord))
        {
            numOfFailedTests++;
            currentTestStatus = TestStatus::ValidationFailed;
            saveFailedTestData(testWord, hypothesis);
            return;
        }
    }

    if (params.savePassedTestData)
    {
        saveTestData(hypothesis, "PASSED");
    }
}

void Tester::testOutputWithEquivalenceCheck(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    LOG("testOutputWithEquivalenceCheck");
    auto combiner = std::make_shared<teacher::AutomataCombiner<AutomatonKind::Combined>>(
        vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
    auto emptinessChecker = std::make_shared<teacher::EmptinessChecker>(
        numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

    auto combinedVpa{combiner->combineVPA(*hypothesis)};

    if (*emptinessChecker->check(combinedVpa) != common::Word{})
    {
        numOfFailedTests++;
        currentTestStatus = TestStatus::ValidationFailed;
    }
}

void Tester::printTestStats()
{
    MeasurementDataBase::printInfo();

    std::cout << "\n[" << (100 * numOfExecutedTests / numOfTests) << "%]"
              << "\033[33m Test stats: number of executed test: " << numOfExecutedTests << "/"
              << numOfTests << ",\033[32m passed: "
              << numOfExecutedTests - numOfFailedTests - numOfHangouts - numOfChildErrors -
                     numOfErrInHandleStackContent - numOfGeneratorCheckFailed
              << ",\033[31m failed:" << numOfFailedTests
              << ", generator check failed: " << numOfGeneratorCheckFailed
              << ", hangouts: " << numOfHangouts << ", child errors: " << numOfChildErrors
              << ", stackContentDiverages errors: " << numOfErrInHandleStackContent << "\033[0m"
              << std::endl;
}

void Tester::setOutputFile()
{
    std::string outputFilePath =
        directoryPath + "output_" + std::to_string(numOfExecutedTests) + ".txt";
    outputFile.open(outputFilePath);

    if (not outputFile)
    {
        ERR("[Tester]: Could not create file!");
        exit(toExit(ExitCode::UNDEFINED));
    }
}

void Tester::printUt()
{
    outputFile << "\nPrinting UT:\n";
    vpa->printUt(outputFile);
}

void Tester::printAutomaton(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> automaton, const std::string &name)
{
    outputFile << "\n" + name + ":\n";
    automaton->print(outputFile);
}

void Tester::saveFailedTestData(
    const std::shared_ptr<common::Word> testWord,
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    setOutputFile();
    outputFile << "test_" << numOfExecutedTests << " details:\n\n";

    printTestDetails(outputFile);
    printAutomaton(vpa, "vpa");
    printAutomaton(hypothesis, "hypothesis");
    outputFile << "\ntestWord:\n" << *testWord << std::endl;
    printUt();

    outputFile.close();
}

void Tester::saveTestData(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const std::string &msg)
{
    setOutputFile();
    outputFile << msg + "\n";
    outputFile << "test_" << numOfExecutedTests << " details:\n\n";

    printTestDetails(outputFile);
    printAutomaton(vpa, "vpa");
    printAutomaton(hypothesis, "hypothesis");
    printUt();

    outputFile.close();
}

void Tester::saveFailedTestData(std::string errMsg)
{
    setOutputFile();

    outputFile << errMsg << "\n\n";
    outputFile << "test_" << numOfExecutedTests << " details:\n\n";

    printTestDetails(outputFile);
    printAutomaton(vpa, "vpa");
    printUt();

    outputFile.close();
}
