#pragma once

#include <iostream>

namespace helper
{
inline void printHelp()
{
    // clang-format off
    std::cout << 
R"(Usage:
    ./run <generator> <seed> <numOfTests>
    ./run bench <scenario>
    ./run custom
    ./run --help
    
Generators:
    random     run tests with random generator
    cda        run tests with CDA generator
    sevpa      run tests with SeVPA generator
    mevpa      run tests with MeVPA generator
    ecda       run tests with eCDA generator
    
Other commands:
    bench      run benchmark for selected scenario
    custom     run example scenario

Scenarios:
    All benchmark scenarios are defined in `benchmark/scenario/scenarios.hpp`

Arguments:
    seed         random seed
    numOfTests   number of tests to run
    scenario     benchmark scenario name

Examples:
    ./run random 42 100
    ./run cda 123 50
    ./run bench base-scenario
    ./run custom)"
    << std::endl;
    // clang-format on
}
} // namespace helper
