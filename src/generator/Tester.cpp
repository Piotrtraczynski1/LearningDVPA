#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "generator/Tester.hpp"
#include "learner/Learner.hpp"
#include "teacher/Converter.hpp"
#include "teacher/Teacher.hpp"
#include "utils/TimeMarker.hpp"

namespace generator
{
Tester::Tester(const uint64_t numOfTests) : numOfTests{numOfTests}
{
    std::srand(std::time(nullptr));
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

        auto res = runTestWithTimeout(std::chrono::seconds(120));
        numOfExecutedTests++;
        switch (res.kind)
        {
        case RunResultKind::Ok:
            testOutput(res.hyp);
            break;
        case RunResultKind::TimeOut:
            ERR("TIMEOUT after %ld ms", res.elapsed.count());
            numOfFailedTests++;
            numOfHangouts++;
            saveFailedTestData("HANGOUT DETECTED!");
            break;
        case RunResultKind::ChildError:
            ERR("Child exited early, after %ld ms", res.elapsed.count());
            numOfFailedTests++;
            numOfChildErrors++;
            saveFailedTestData("CHILD ERROR!");
            break;
        }

        printTestStats();

        generator.reset();
    }
}

void Tester::prepareTest()
{
    numOfStates = (rand() % 8) + 1;
    numOfCalls = (rand() % 4) + 0;
    numOfLocals = (rand() % 4) + 0;
    numOfReturns = (rand() % 4) + 0;
    numOfStackSymbols = (rand() % 6) + 2;

    generator = std::make_unique<generator::Generator>(
        numOfStates, numOfCalls, numOfLocals, numOfReturns, numOfStackSymbols);
    vpa = generator->run();
}

void Tester::printTestDetails()
{
    std::cout << "Prepared test number: " << numOfExecutedTests << "\n numOfStates: " << numOfStates
              << ", numOfCalls: " << numOfCalls << ", numOfLocals:" << numOfLocals
              << ", numOfReturns" << numOfReturns << ", numOfStackSymbols: " << numOfStackSymbols
              << std::endl;
}

RunResult Tester::runTestWithTimeout(std::chrono::milliseconds timeout)
{
    using clock = std::chrono::steady_clock;
    auto t0 = clock::now();

    pid_t pid = fork();

    if (pid == 0)
    {
        auto converter = std::make_shared<teacher::Converter>(
            vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
        teacher::Teacher teacher{vpa, converter};
        learner::Learner learner{teacher, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols};
        (void)learner.run();
        _exit(0);
    }

    const auto deadline = clock::now() + timeout;
    int status = 0;
    while (true)
    {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r == pid)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                auto converter = std::make_shared<teacher::Converter>(
                    vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
                teacher::Teacher teacher{vpa, converter};
                learner::Learner learner{
                    teacher, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols};
                return {RunResultKind::Ok, learner.run(), elapsed};
            }
            else
            {
                return {RunResultKind::ChildError, nullptr, elapsed};
            }
        }

        if (clock::now() >= deadline)
        {
            kill(pid, SIGKILL);
            (void)waitpid(pid, &status, 0);
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t0);
            return {RunResultKind::TimeOut, nullptr, elapsed};
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

std::shared_ptr<common::Word> Tester::generateRandomWord(uint16_t length)
{
    common::Word word{};

    for (uint16_t i = 0; i < length; i++)
    {
        uint16_t symol{static_cast<uint16_t>(rand() % 3)};

        switch (symol)
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
            std::cout << "ERROR";
        }
    }

    return std::make_shared<common::Word>(word);
}

void Tester::testOutput(const std::shared_ptr<common::VPA> hypothesis)
{
    TIME_MARKER("testOutput");
    for (int i = 0; i < 5000; i++)
    {
        const uint16_t length{static_cast<uint16_t>(rand() % 25)};
        std::shared_ptr<common::Word> testWord{generateRandomWord(length)};

        if (hypothesis->checkWord(*testWord) != vpa->checkWord(*testWord))
        {
            numOfFailedTests++;
            saveFailedTestData(testWord, hypothesis);
            return;
        }
    }
}

void Tester::printTestStats()
{
    MeasurementDataBase::printInfo();

    std::cout << "\n[" << (100 * numOfExecutedTests / numOfTests) << "%]"
              << "\033[33m Test stats: number of executed test: " << numOfExecutedTests << "/"
              << numOfTests << ",\033[32m passed: " << numOfExecutedTests - numOfFailedTests
              << ",\033[31m failed:" << numOfFailedTests << ", hangouts: " << numOfHangouts
              << ", child errors: " << numOfChildErrors << "\033[0m" << std::endl;
}

void Tester::saveFailedTestData(
    const std::shared_ptr<common::Word> testWord, const std::shared_ptr<common::VPA> hypothesis)
{
    std::string outputFile = directoryPath + "output_" + std::to_string(numOfFailedTests) + ".txt";
    std::ofstream file(outputFile);

    if (!file)
    {
        std::cerr << "Could not create file!\n";
        exit(1);
    }

    file << "test_" << numOfFailedTests << " details:\n\n";

    file << "numOfStates: " << numOfStates << ", numOfCalls: " << numOfCalls
         << ", numOfLocals:" << numOfLocals << ", numOfReturns: " << numOfReturns
         << ", numOfStackSymbols: " << numOfStackSymbols << "\n\n";

    file << "vpa:\n";
    vpa->print(file);

    file << "\nhypothesis:\n";
    hypothesis->print(file);

    file << "\ntestWord:\n" << *testWord << std::endl;

    file << "\nPrinting UT:\n";

    vpa->printUt(file);

    file.close();
}

void Tester::saveFailedTestData(std::string errMsg)
{
    std::string outputFile = directoryPath + "output_" + std::to_string(numOfFailedTests) + ".txt";
    std::ofstream file(outputFile);

    if (!file)
    {
        std::cerr << "Nie można utworzyć pliku!\n";
        exit(1);
    }

    file << errMsg << "\n\n";

    file << "test_" << numOfFailedTests << " details:\n\n";

    file << "numOfStates: " << numOfStates << ", numOfCalls: " << numOfCalls
         << ", numOfLocals:" << numOfLocals << ", numOfReturns: " << numOfReturns
         << ", numOfStackSymbols: " << numOfStackSymbols << "\n\n";

    file << "vpa:\n";
    vpa->print(file);

    file << "\nPrinting UT:\n";

    vpa->printUt(file);

    file.close();
}
} // namespace generator