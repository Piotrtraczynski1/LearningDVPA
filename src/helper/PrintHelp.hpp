#pragma once

#include <iostream>

namespace helper
{
inline void printHelp()
{
    // clang-format off
    std::cout << 
R"(Usage:
    ./run <command> <seed> <numOfTests>
    ./run custom
    ./run --help
    
Commands:
    random     run tests with random generator
    cda        run tests with CDA generator
    sevpa      run tests with SeVPA generator
    mevpa      run tests with MeVPA generator
    ecda       run tests with eCDA generator
    custom     run example scenario

Arguments:
    seed         random seed
    numOfTests   number of tests to run

Examples:
    ./run random 42 100
    ./run cda 123 50
    ./run custom)"
    << std::endl;
    // clang-format on
}
} // namespace helper
