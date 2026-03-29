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
} // namespace

std::tuple<Command, int, uint16_t> parseInput(int argc, char *argv[])
{
    constexpr uint16_t dummyNumber{0};

    if (argc < 2)
    {
        return {Command::InvalidNumOfArg, dummyNumber, dummyNumber};
    }

    std::string cmd = argv[1];
    Command command{parseCommand(cmd)};

    uint16_t numOfTests{};
    int seed{};

    if (command == Command::Random or command == Command::Cda or command == Command::SeVpa or
        command == Command::MeVpa or command == Command::eCda)
    {
        if (argc < 4)
        {
            return {Command::InvalidNumOfArg, dummyNumber, dummyNumber};
        }
        numOfTests = std::stoi(argv[3]);
        seed = std::stoi(argv[2]);
    }

    return {command, seed, numOfTests};
}
} // namespace helper