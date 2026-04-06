#include "Tester.hpp"
#include "TesterParameters.hpp"
#include "benchmark/BenchmarkRunner.hpp"
#include "generator/Generators.hpp"
#include "helper/Command.hpp"
#include "helper/ExampleRun.hpp"
#include "helper/InputParser.hpp"
#include "helper/PrintHelp.hpp"

namespace
{
void runRandomGenerator(const uint16_t numOfTests)
{
    Tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::RandomGenerator()),
        RandomTestParameters}
        .run();
}

void runCdaGenerator(const uint16_t numOfTests)
{
    Tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::CdaGenerator()),
        CdaTestParameters}
        .run();
}

void runSeVpaGenerator(const uint16_t numOfTests)
{
    Tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::SeVpaGenerator()),
        SeVpaTestParameters}
        .run();
}

void runMeVpaGenerator(const uint16_t numOfTests)
{
    Tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::MeVpaGenerator()),
        MeVpaTestParameters}
        .run();
}

void runECdaGenerator(const uint16_t numOfTests)
{
    Tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::ECdaGenerator()),
        ECdaTestParameters}
        .run();
}

void runCombinedGenerator(const uint16_t numOfTests)
{
    Tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::CombinedGenerator()),
        CombinedTestParameters}
        .run();
}

void runBenchmark(const std::string &scenarioName)
{
    benchmark::BenchmarkRunner{scenarioName}.run();
}
} // namespace

int main(int argc, char *argv[])
{
    auto [command, seed, numOfTests, scenarioName] = helper::parseInput(argc, argv);

    switch (command)
    {
    case (helper::Command::Random):
        std::srand(seed);
        runRandomGenerator(numOfTests);
        break;
    case (helper::Command::Combined):
        std::srand(seed);
        runCombinedGenerator(numOfTests);
        break;
    case (helper::Command::Cda):
        std::srand(seed);
        runCdaGenerator(numOfTests);
        break;
    case (helper::Command::SeVpa):
        std::srand(seed);
        runSeVpaGenerator(numOfTests);
        break;
    case (helper::Command::MeVpa):
        std::srand(seed);
        runMeVpaGenerator(numOfTests);
        break;
    case (helper::Command::eCda):
        std::srand(seed);
        runECdaGenerator(numOfTests);
        break;
    case (helper::Command::Custom):
        helper::runExample();
        break;
    case (helper::Command::Benchmark):
        runBenchmark(scenarioName);
        break;
    case (helper::Command::Help):
        helper::printHelp();
        break;
    case (helper::Command::InvalidNumOfArg):
        ERR("Invalid number of arguments");
        helper::printHelp();
        return 1;
    default:
        ERR("Unknown command");
        helper::printHelp();
        return 1;
    }

    return 0;
}