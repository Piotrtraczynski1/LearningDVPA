#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "benchmark/scenario/Scenario.hpp"

namespace benchmark
{
class BenchmarkRunner
{
public:
    BenchmarkRunner(const std::string &scenarioName);
    void run();

private:
    void saveResults(const uint16_t numOfTestsInSingleIteration);

    void createDirectory();
    std::string getDirectoryName();
    std::string getOutputFileName();
    std::string getPlotFileName(const std::string &marker);
    std::string getHeatmapFileName(const std::string &marker);
    void saveMarker(
        std::ofstream &out, const std::string &marker, const uint16_t numOfTestsInSingleIteration);
    void saveHeatmap(const std::string &marker, const uint16_t numOfTestsInSingleIteration);
    void savePlotExecutions(const std::string &marker, const uint16_t numOfTestsInSingleIteration);
    void savePlotTime(const std::string &marker, const uint16_t numOfTestsInSingleIteration);

    static const std::vector<std::string> markersToBeIncluded;
    static const std::vector<std::string> heatmapMarkers;
    static const std::vector<std::string> plotExecutionMarkers;
    static const std::vector<std::string> plotTimeMarkers;

    std::string scenarioName;
    std::unique_ptr<scenario::Scenario> scenario;
};
} // namespace benchmark