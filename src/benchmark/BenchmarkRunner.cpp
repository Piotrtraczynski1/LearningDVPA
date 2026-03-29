#include <filesystem>
#include <fstream>
#include <iostream>

#include "benchmark/BenchmarkRunner.hpp"
#include "benchmark/scenario/Scenarios.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace benchmark
{
// clang-format off
const std::vector<std::string> BenchmarkRunner::markersToBeIncluded = {
    "[Teacher]: membershipQuery",
    "[Teacher]: stackContentQuery",
    "[Teacher]: equivalenceQuery",
    "[SrsChecker]: check consistency with SRS",
    "[SrsChecker]: counterexample found",
    "[AutomataGenerator]: generateAutomaton"
};
// clang-format on

BenchmarkRunner::BenchmarkRunner(const std::string &scenarioNameArg) : scenarioName{scenarioNameArg}
{
}

void BenchmarkRunner::run()
{
    auto it = benchmark::scenariosRegistry.find(scenarioName);
    if (it == scenariosRegistry.end())
    {
        ERR("Unknown scenario: %s", scenarioName.c_str());
        return;
    }

    createDirectory();

    auto scenario = it->second();

    std::srand(scenario->getSeed());
    const auto numOfTestsInSingleIteration{scenario->getNumOfTestsInSingleIteration()};
    for (uint16_t iteration = 0; iteration < scenario->getNumOfIterations(); iteration++)
    {
        scenario->runSingleIteration();
        saveResults(iteration, numOfTestsInSingleIteration);
        MeasurementDataBase::reset();
    }
}

void BenchmarkRunner::createDirectory()
{
    std::filesystem::create_directories(getDirectoryName());
    std::ofstream out(getOutputFileName(), std::ios::trunc);
    out << "iteration;marker;time;executions;avg_time_per_execution\n";
}

std::string BenchmarkRunner::getDirectoryName()
{
    return (std::filesystem::path(__FILE__).parent_path() / "results" / scenarioName).string();
}

std::string BenchmarkRunner::getOutputFileName()
{
    return (std::filesystem::path(getDirectoryName()) / "output.csv").string();
}

void BenchmarkRunner::saveResults(
    const uint16_t iteration, const uint16_t numOfTestsInSingleIteration)
{
    std::ofstream out(getOutputFileName(), std::ios::app);
    for (const auto &marker : markersToBeIncluded)
    {
        saveMarker(out, marker, iteration, numOfTestsInSingleIteration);
    }
}

void BenchmarkRunner::saveMarker(
    std::ofstream &out, const std::string &marker, const uint16_t iteration,
    const uint16_t numOfTestsInSingleIteration)
{
    const auto markerInfo{MeasurementDataBase::getMarkerInfo(marker)};
    if (markerInfo.executions > 0)
    {
        out << iteration << ";" << marker << ";" << (markerInfo.time / numOfTestsInSingleIteration)
            << ";" << (markerInfo.executions / numOfTestsInSingleIteration) << ";"
            << (markerInfo.time / markerInfo.executions) << "\n";
    }
}
} // namespace benchmark