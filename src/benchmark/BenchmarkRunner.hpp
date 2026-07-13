#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include "benchmark/scenario/Scenario.hpp"

namespace benchmark
{
class BenchmarkRunner
{
public:
    BenchmarkRunner(const std::string &scenarioName);
    void run();

private:
    void createDirectory();
    std::string getDirectoryName() const;
    std::string getRunsFileName() const;
    void writeHeader(std::ofstream &out) const;
    void saveResult(std::ofstream &out, uint16_t runIndex, const SingleTestResult &result) const;

    std::string scenarioName;
    std::unique_ptr<scenario::Scenario> scenario;
};
} // namespace benchmark
