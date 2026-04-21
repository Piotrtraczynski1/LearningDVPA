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
void runRandomGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::RandomGenerator()),
        RandomTestParameters, seed}
        .run();
}

void runCdaGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests,
        std::unique_ptr<generator::Generator>(new generator::callDriven::CdaGenerator()),
        CdaTestParameters, seed}
        .run();
}

void runSeVpaGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests,
        std::unique_ptr<generator::Generator>(new generator::callDriven::SeVpaGenerator()),
        SeVpaTestParameters, seed}
        .run();
}

void runMeVpaGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests,
        std::unique_ptr<generator::Generator>(new generator::callDriven::MeVpaGenerator()),
        MeVpaTestParameters, seed}
        .run();
}

void runECdaGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests,
        std::unique_ptr<generator::Generator>(new generator::callDriven::ECdaGenerator()),
        ECdaTestParameters, seed}
        .run();
}

void runCommutativeGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests,
        std::unique_ptr<generator::Generator>(new generator::srs::CommutativeGenerator()),
        CommutativeTestParameters, seed}
        .run();
}

void runCancellationGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests,
        std::unique_ptr<generator::Generator>(new generator::srs::CancellationGenerator()),
        CancellationTestParameters, seed}
        .run();
}

void runIdempotencyGenerator(const uint16_t numOfTests, const uint32_t seed)
{
    Tester{
        numOfTests,
        std::unique_ptr<generator::Generator>(new generator::srs::IdempotencyGenerator()),
        IdempotencyTestParameters, seed}
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
        runRandomGenerator(numOfTests, seed);
        break;
    case (helper::Command::Commutative):
        runCommutativeGenerator(numOfTests, seed);
        break;
    case (helper::Command::Cancellation):
        runCancellationGenerator(numOfTests, seed);
        break;
    case (helper::Command::Idempotency):
        runIdempotencyGenerator(numOfTests, seed);
        break;
    case (helper::Command::Cda):
        runCdaGenerator(numOfTests, seed);
        break;
    case (helper::Command::SeVpa):
        runSeVpaGenerator(numOfTests, seed);
        break;
    case (helper::Command::MeVpa):
        runMeVpaGenerator(numOfTests, seed);
        break;
    case (helper::Command::eCda):
        runECdaGenerator(numOfTests, seed);
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