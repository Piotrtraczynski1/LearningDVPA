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

const std::vector<std::string> BenchmarkRunner::heatmapMarkers = {
    "[Teacher]: membershipQuery",
    "[Teacher]: stackContentQuery",
    "[Teacher]: equivalenceQuery"
};

const std::vector<std::string> BenchmarkRunner::plotExecutionMarkers = {
    "[Teacher]: equivalenceQuery",
    "[SrsChecker]: counterexample found"
};

const std::vector<std::string> BenchmarkRunner::plotTimeMarkers = {
    "[Learner]: Learner::run"
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

    scenario = it->second();

    std::srand(scenario->getSeed());
    const auto numOfTestsInSingleIteration{scenario->getNumOfTestsInSingleIteration()};
    for (uint16_t iteration4Dim = 0; iteration4Dim < scenario->getNumOfIterationsIn4Dim();
         iteration4Dim++)
    {
        for (uint16_t iteration3Dim = 0; iteration3Dim < scenario->getNumOfIterationsIn3Dim();
             iteration3Dim++)
        {
            for (uint16_t iteration2Dim = 0; iteration2Dim < scenario->getNumOfIterationsIn2Dim();
                 iteration2Dim++)
            {
                for (uint16_t iteration1Dim = 0;
                     iteration1Dim < scenario->getNumOfIterationsIn1Dim(); iteration1Dim++)
                {
                    scenario->runSingleIteration();
                    saveResults(numOfTestsInSingleIteration);
                    MeasurementDataBase::reset();
                    scenario->prepareNextIterationDim1();
                }
                scenario->resetDim1();
                scenario->prepareNextIterationDim2();
            }
            scenario->resetDim2();
            scenario->prepareNextIterationDim3();
        }
        scenario->resetDim3();
        scenario->prepareNextIterationDim4();
    }
}

void BenchmarkRunner::createDirectory()
{
    std::filesystem::create_directories(getDirectoryName());
    std::ofstream out(getOutputFileName(), std::ios::trunc);
    out << "dim4Name;dim4Details;dim3Name;dim3Details;dim2Name;dim2Details;dim1Name;dim1Details;"
           "marker;time;executions;avg_time_per_execution\n";

    for (const auto &marker : heatmapMarkers)
    {
        std::ofstream out(getHeatmapFileName(marker), std::ios::trunc);
    }

    for (const auto &marker : plotExecutionMarkers)
    {
        std::ofstream out(getPlotFileName(marker), std::ios::trunc);
    }

    for (const auto &marker : plotTimeMarkers)
    {
        std::ofstream out(getPlotFileName(marker), std::ios::trunc);
    }
}

std::string BenchmarkRunner::getDirectoryName()
{
    return (std::filesystem::path(__FILE__).parent_path() / "results" / scenarioName).string();
}

std::string BenchmarkRunner::getOutputFileName()
{
    return (std::filesystem::path(getDirectoryName()) / "output.csv").string();
}

std::string BenchmarkRunner::getHeatmapFileName(const std::string &marker)
{
    std::string filename = "heatmap" + marker + ".txt";
    return (std::filesystem::path(getDirectoryName()) / filename).string();
}

std::string BenchmarkRunner::getPlotFileName(const std::string &marker)
{
    std::string filename = "plot" + marker + ".txt";
    return (std::filesystem::path(getDirectoryName()) / filename).string();
}

void BenchmarkRunner::saveResults(const uint16_t numOfTestsInSingleIteration)
{
    std::ofstream out(getOutputFileName(), std::ios::app);
    for (const auto &marker : markersToBeIncluded)
    {
        saveMarker(out, marker, numOfTestsInSingleIteration);
    }

    for (const auto &marker : heatmapMarkers)
    {
        saveHeatmap(marker, numOfTestsInSingleIteration);
    }

    for (const auto &marker : plotExecutionMarkers)
    {
        savePlotExecutions(marker, numOfTestsInSingleIteration);
    }

    for (const auto &marker : plotTimeMarkers)
    {
        savePlotTime(marker, numOfTestsInSingleIteration);
    }
}

void BenchmarkRunner::saveMarker(
    std::ofstream &out, const std::string &marker, const uint16_t numOfTestsInSingleIteration)
{
    const auto markerInfo{MeasurementDataBase::getMarkerInfo(marker)};
    if (markerInfo.executions > 0)
    {
        out << scenario->getDim4Name() << ";" << scenario->getDim4Details() << ";"
            << scenario->getDim3Name() << ";" << scenario->getDim3Details() << ";"
            << scenario->getDim2Name() << ";" << scenario->getDim2Details() << ";"
            << scenario->getDim1Name() << ";" << scenario->getDim1Details() << ";" << marker << ";"
            << (markerInfo.time / numOfTestsInSingleIteration) << ";"
            << (static_cast<float>(markerInfo.executions) / numOfTestsInSingleIteration) << ";"
            << (markerInfo.time / markerInfo.executions) << "\n";
    }
}

void BenchmarkRunner::saveHeatmap(
    const std::string &marker, const uint16_t numOfTestsInSingleIteration)
{
    std::ofstream out(getHeatmapFileName(marker), std::ios::app);
    const auto markerInfo{MeasurementDataBase::getMarkerInfo(marker)};
    if (markerInfo.executions > 0)
    {
        out << marker << " (" << scenario->getDim2Details() << ", " << scenario->getDim1Details()
            << ") [" << (static_cast<float>(markerInfo.executions) / numOfTestsInSingleIteration)
            << "]\n";
    }
}

void BenchmarkRunner::savePlotExecutions(
    const std::string &marker, const uint16_t numOfTestsInSingleIteration)
{
    std::ofstream out(getPlotFileName(marker), std::ios::app);
    const auto markerInfo{MeasurementDataBase::getMarkerInfo(marker)};
    if (markerInfo.executions > 0)
    {
        out << marker << ", dim2Details: " << scenario->getDim2Details() << ", ("
            << scenario->getDim1Details() << ", "
            << (static_cast<float>(markerInfo.executions) / numOfTestsInSingleIteration) << ")\n";
    }
}

void BenchmarkRunner::savePlotTime(
    const std::string &marker, const uint16_t numOfTestsInSingleIteration)
{
    constexpr double microsecondsToSeconds{1e-6};

    std::ofstream out(getPlotFileName(marker), std::ios::app);
    const auto markerInfo{MeasurementDataBase::getMarkerInfo(marker)};
    if (markerInfo.executions > 0)
    {
        out << marker << ", dim2Details: " << scenario->getDim2Details() << ", ("
            << scenario->getDim1Details() << ", "
            << (static_cast<float>(markerInfo.time) / numOfTestsInSingleIteration) *
                   microsecondsToSeconds
            << ")\n";
    }
}
} // namespace benchmark