#include <cstdlib>
#include <thread>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Tester.hpp"
#include "learner/Srs.hpp"
#include "teacher/Converter.hpp"
#include "utils/TimeMarker.hpp"

Tester::Tester(
    const uint64_t numOfTests, std::unique_ptr<generator::Generator> gen,
    const TesterParameters &testerParameters)
    : numOfTests{numOfTests}, generator{std::move(gen)}, params{testerParameters}
{
    setOutputDirectoryPath();
}

void Tester::setOutputDirectoryPath()
{
    std::string path = std::string(__FILE__);
    path = path.substr(0, path.find_last_of("/\\") + 1);
    directoryPath = path + "failedTests/";
}

void Tester::run()
{
    while (numOfExecutedTests < numOfTests)
    {
        prepareTest();
        printTestDetails();

        if (params.supervisedMode)
        {
            runSupervisedTest();
        }
        else
        {

            testOutput(runLearner());
        }

        numOfExecutedTests++;
        printTestStats();
    }
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> Tester::runLearner()
{
    converter = std::make_shared<teacher::Converter>(
        vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
    teacher = std::make_shared<teacher::Teacher>(vpa, converter);
    learner::Srs srs = params.useSrs ? generator->getSrs() : learner::Srs{};
    learner = std::make_shared<learner::Learner>(
        *teacher, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols, srs);

    return learner->run();
}

void Tester::runSupervisedTest()
{
    auto res = runTestWithTimeout();
    switch (res.exitCode)
    {
    case ExitCode::OK:
        testOutput(res.hyp);
        break;
    case ExitCode::TIMEOUT:
        ERR("[Tester]: TIMEOUT after %ld ms", res.elapsed.count());
        numOfHangouts++;
        saveFailedTestData("HANGOUT DETECTED!");
        break;
    case ExitCode::STACKCONTENTDIVERGE:
        ERR("[Tester]: ERROR in StackContentDiverages, after %ld ms", res.elapsed.count());
        numOfErrInHandleStackContent++;
        saveFailedTestData("StackContentDiverages ERROR!");
        break;
    default:
        ERR("[Tester]: Child exited early, after %ld ms with error: %s", res.elapsed.count(),
            toString(res.exitCode));
        numOfChildErrors++;
        saveFailedTestData("CHILD ERROR: " + std::string(toString(res.exitCode)));
        break;
    }
}

void Tester::prepareTest()
{
    numOfStates =
        (rand() % (params.maxNumOfStates - params.minNumOfStates + 1)) + params.minNumOfStates;
    numOfCalls =
        (rand() % (params.maxNumOfCalls - params.minNumOfCalls + 1)) + params.minNumOfCalls;
    numOfLocals =
        (rand() % (params.maxNumOfLocals - params.minNumOfLocals + 1)) + params.minNumOfLocals;
    numOfReturns =
        (rand() % (params.maxNumOfReturns - params.minNumOfReturns + 1)) + params.minNumOfReturns;
    numOfStackSymbols = (rand() % (params.maxNumOfStackSymbols - params.minNumOfStackSymbols + 1)) +
                        params.minNumOfStackSymbols;

    generator->setConfig(
        numOfStates, numOfCalls, numOfLocals, numOfReturns, numOfStackSymbols, params.density,
        params.acceptingStatesDensity, params.numOfModules);
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
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                return {ExitCode::OK, runLearner(), elapsed};
            }
            else
            {
                return {ExitCode{WEXITSTATUS(status)}, nullptr, elapsed};
            }
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
        uint8_t symbol{static_cast<uint8_t>(rand() % 3)};

        switch (symbol)
        {
        case 0:
            if (numOfCalls != 0)
            {
                word += common::Word{
                    common::symbol::CallSymbol{static_cast<uint16_t>(rand() % numOfCalls)}};
            }
            break;
        case 1:
            if (numOfLocals != 0)
            {
                word += common::Word{
                    common::symbol::LocalSymbol{static_cast<uint16_t>(rand() % numOfLocals)}};
            }
            break;
        case 2:
            if (numOfReturns != 0)
            {
                word += common::Word{
                    common::symbol::ReturnSymbol{static_cast<uint16_t>(rand() % numOfReturns)}};
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
    if (not generator->generatorSpecificCheck(hypothesis))
    {
        ERR("[Tester]: Generator specific check failed!");
        numOfGeneratorCheckFailed++;
        saveTestData(hypothesis, "GENERATOR SPECIFIC CHECK FAILED");
        return;
    }

    for (uint16_t i = 0; i < params.numOfRandomTestingWords; i++)
    {
        const uint16_t length{static_cast<uint16_t>(rand() % (params.maxTestingWordLength + 1))};
        std::shared_ptr<common::Word> testWord{generateRandomWord(length)};

        if (hypothesis->checkWord(*testWord) != vpa->checkWord(*testWord))
        {
            numOfFailedTests++;
            saveFailedTestData(testWord, hypothesis);
            return;
        }
    }

    if (params.savePassedTestData)
    {
        saveTestData(hypothesis, "PASSED");
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