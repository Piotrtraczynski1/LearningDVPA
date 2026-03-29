#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace benchmark
{
class BenchmarkRunner
{
public:
    BenchmarkRunner(const std::string &scenarioName);
    void run();

private:
    void saveResults(const uint16_t iteration, const uint16_t numOfTestsInSingleIteration);

    void createDirectory();
    std::string getDirectoryName();
    std::string getOutputFileName();
    void saveMarker(
        std::ofstream &out, const std::string &marker, const uint16_t iteration,
        const uint16_t numOfTestsInSingleIteration);

    static const std::vector<std::string> markersToBeIncluded;
    std::string scenarioName;
};

} // namespace benchmark