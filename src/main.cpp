#include <ctime>
#include <random>

#include "Tester.hpp"
#include "TesterParameters.hpp"
#include "generator/Generators.hpp"

#include "common/transition/Transition.hpp"

void runRandomGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::RandomGenerator()),
        RandomTestParameters};
    tester.run();
}

void runCdaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::CdaGenerator()),
        CdaTestParameters};
    tester.run();
}

void runSeVpaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::SeVpaGenerator()),
        SeVpaTestParameters};
    tester.run();
}

void runMeVpaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::MeVpaGenerator()),
        MeVpaTestParameters};
    tester.run();
}

void runECdaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::ECdaGenerator()),
        ECdaTestParameters};
    tester.run();
}

void runCustomTest()
{
    LOG("custom test");
}

void printHelp()
{
    std::cout << "help\n";
}

enum class Command
{
    Random,
    Cda,
    SeVpa,
    MeVpa,
    eCda,
    Custom,
    Help,
    Unknown
};

Command parseCommand(const std::string &cmd)
{
    if (cmd == "random")
        return Command::Random;
    if (cmd == "cda")
        return Command::Cda;
    if (cmd == "sevpa")
        return Command::SeVpa;
    if (cmd == "mevpa")
        return Command::MeVpa;
    if (cmd == "ecda")
        return Command::eCda;
    if (cmd == "custom")
        return Command::Custom;
    if (cmd == "--help" or cmd == "-h")
        return Command::Help;
    return Command::Unknown;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        ERR("Invalid number of arguments");
        printHelp();
        return 1;
    }

    std::string cmd = argv[1];
    Command command{parseCommand(cmd)};

    uint16_t numOfTests{};

    if (command == Command::Random or command == Command::Cda or command == Command::SeVpa or
        command == Command::MeVpa or command == Command::eCda)
    {
        if (argc < 4)
        {
            ERR("Invalid number of arguments");
            printHelp();
            return 1;
        }
        numOfTests = std::stoi(argv[3]);
        int seed = std::stoi(argv[2]);
        std::srand(seed);
    }

    switch (command)
    {
    case (Command::Random):
        runRandomGenerator(numOfTests);
        break;
    case (Command::Cda):
        runCdaGenerator(numOfTests);
        break;
    case (Command::SeVpa):
        runSeVpaGenerator(numOfTests);
        break;
    case (Command::MeVpa):
        runMeVpaGenerator(numOfTests);
        break;
    case (Command::eCda):
        runECdaGenerator(numOfTests);
        break;
    case (Command::Custom):
        runCustomTest();
        break;
    case (Command::Help):
        printHelp();
        break;
    default:
        ERR("Invalid command");
        printHelp();
        return 1;
    }

    return 0;
}