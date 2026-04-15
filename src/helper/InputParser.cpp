#include <string>

#include "helper/InputParser.hpp"
#include "utils/log.hpp"

namespace helper
{

namespace
{
Command parseCommand(const std::string &cmd)
{
    if (cmd == "random")
        return Command::Random;
    if (cmd == "commutative")
        return Command::Commutative;
    if (cmd == "cancel")
        return Command::Cancellation;
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
    if (cmd == "bench")
        return Command::Benchmark;
    if (cmd == "--help" or cmd == "-h")
        return Command::Help;
    return Command::Unknown;
}
} // namespace

std::tuple<Command, int, uint16_t, std::string> parseInput(int argc, char *argv[])
{
    constexpr uint16_t dummyNumber{0};
    const std::string dummyScenario{};

    if (argc < 2)
    {
        return {Command::InvalidNumOfArg, dummyNumber, dummyNumber, dummyScenario};
    }

    std::string cmd = argv[1];
    Command command{parseCommand(cmd)};

    int seed{dummyNumber};
    uint16_t numOfTests{dummyNumber};
    std::string scenarioName{dummyScenario};

    if (command == Command::Benchmark)
    {
        if (argc < 3)
        {
            return {Command::InvalidNumOfArg, dummyNumber, dummyNumber, dummyScenario};
        }
        scenarioName = argv[2];
    }

    if (command == Command::Random or command == Command::Commutative or
        command == Command::Cancellation or command == Command::Cda or command == Command::SeVpa or
        command == Command::MeVpa or command == Command::eCda)
    {
        if (argc < 4)
        {
            return {Command::InvalidNumOfArg, dummyNumber, dummyNumber, dummyScenario};
        }
        seed = std::stoi(argv[2]);
        numOfTests = std::stoi(argv[3]);
    }

    return {command, seed, numOfTests, scenarioName};
}
} // namespace helper