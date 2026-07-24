#include <filesystem>
#include <fstream>

#include "benchmark/BenchmarkRunner.hpp"
#include "benchmark/scenario/Scenarios.hpp"
#include "utils/Counters.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace benchmark
{
namespace
{
constexpr const char *membershipQueryMarker{"[Teacher]: membershipQuery"};
constexpr const char *stackContentQueryMarker{"[Teacher]: stackContentQuery"};
constexpr const char *equivalenceQueryMarker{"[Teacher]: equivalenceQuery"};
constexpr const char *srsCheckMarker{"[SrsChecker]: check consistency with SRS"};
constexpr const char *srsCounterexampleMarker{"[SrsChecker]: counterexample found"};
constexpr const char *automataGenerationMarker{"[AutomataGenerator]: generateAutomaton"};
constexpr const char *learnerMarker{"[Learner]: Learner::run"};
constexpr const char *outputValidationMarker{"[Tester]: testOutput"};
} // namespace

BenchmarkRunner::BenchmarkRunner(const std::string &scenarioNameArg) : scenarioName{scenarioNameArg}
{
}

void BenchmarkRunner::run()
{
    auto it = scenariosRegistry.find(scenarioName);
    if (it == scenariosRegistry.end())
    {
        ERR("Unknown scenario: %s", scenarioName.c_str());
        return;
    }

    scenario = it->second();
    createDirectory();

    std::ofstream out(getRunsFileName(), std::ios::trunc);
    if (not out)
    {
        ERR("Could not create benchmark results file: %s", getRunsFileName().c_str());
        return;
    }
    writeHeader(out);

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
                    for (uint16_t runIndex = 0;
                         runIndex < scenario->getNumOfTestsInSingleIteration(); runIndex++)
                    {
                        MeasurementDataBase::reset();
                        Counters::reset();
                        const uint32_t seed = static_cast<uint32_t>(scenario->getSeed()) + runIndex;
                        saveResult(out, runIndex, scenario->runSingle(seed));
                    }
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
}

std::string BenchmarkRunner::getDirectoryName() const
{
    return (std::filesystem::path(__FILE__).parent_path() / "results" / scenarioName).string();
}

std::string BenchmarkRunner::getRunsFileName() const
{
    return (std::filesystem::path(getDirectoryName()) / "runs.csv").string();
}

void BenchmarkRunner::writeHeader(std::ofstream &out) const
{
    out << "scenario,runIndex,seed,generator,numOfStates,numOfCalls,numOfLocals,numOfReturns,"
           "numOfStackSymbols,density,acceptingStatesDensity,numOfModules,secondDvpaNumOfStates,"
           "useSrs,useEquivalenceCheckToValidateOutput,equivalenceQueryCount,"
           "equivalenceQueryHypothesisStatesTotal,equivalenceQueryTimeUs,"
           "equivalenceQueryTargetMembershipQueryCount,"
           "equivalenceQueryTargetMembershipQuerySymbolsTotal,"
           "membershipQueryCount,"
           "membershipQuerySymbolsTotal,membershipQueryTimeUs,stackContentQueryCount,"
           "stackContentQuerySymbolsTotal,stackContentQueryTimeUs,srsCheckCount,srsCheckTimeUs,"
           "srsCounterexampleCount,automataGeneratedCount,automataGenerationTimeUs,learnerTimeUs,"
           "outputValidationTimeUs,status,targetNumOfStates,targetAcceptingStates,"
           "hypothesisNumOfStates,hypothesisAcceptingStates\n";
}

void BenchmarkRunner::saveResult(
    std::ofstream &out, uint16_t runIndex, const SingleTestResult &result) const
{
    const auto &parameters = scenario->getParameters();
    const auto membershipQuery = MeasurementDataBase::getMarkerInfo(membershipQueryMarker);
    const auto stackContentQuery = MeasurementDataBase::getMarkerInfo(stackContentQueryMarker);
    const auto equivalenceQuery = MeasurementDataBase::getMarkerInfo(equivalenceQueryMarker);
    const auto srsCheck = MeasurementDataBase::getMarkerInfo(srsCheckMarker);
    const auto srsCounterexample = MeasurementDataBase::getMarkerInfo(srsCounterexampleMarker);
    const auto automataGeneration = MeasurementDataBase::getMarkerInfo(automataGenerationMarker);
    const auto learner = MeasurementDataBase::getMarkerInfo(learnerMarker);
    const auto outputValidation = MeasurementDataBase::getMarkerInfo(outputValidationMarker);

    out << scenarioName << "," << runIndex << "," << result.seed << ","
        << scenario->getGeneratorName() << "," << result.numOfStates << "," << result.numOfCalls
        << "," << result.numOfLocals << "," << result.numOfReturns << ","
        << result.numOfStackSymbols << "," << parameters.density << ","
        << parameters.acceptingStatesDensity << "," << parameters.numOfModules << ","
        << result.secondDvpaNumOfStates << "," << parameters.useSrs << ","
        << parameters.useEquivalenceCheckToValidateOutput << "," << equivalenceQuery.executions
        << "," << Counters::getCounter("equivalenceQuery") << "," << equivalenceQuery.time << ","
        << Counters::getCounter("equivalenceQueryTargetMembershipQuery") << ","
        << Counters::getCounter("equivalenceQueryTargetMembershipQuerySymbols") << ","
        << membershipQuery.executions << "," << Counters::getCounter("membershipQuery") << ","
        << membershipQuery.time << "," << stackContentQuery.executions << ","
        << Counters::getCounter("stackContentQuery") << "," << stackContentQuery.time << ","
        << srsCheck.executions << "," << srsCheck.time << "," << srsCounterexample.executions << ","
        << automataGeneration.executions << "," << automataGeneration.time << "," << learner.time
        << "," << outputValidation.time << "," << toString(result.status) << ","
        << result.targetNumOfStates << "," << result.targetAcceptingStates << ","
        << result.hypothesisNumOfStates << "," << result.hypothesisAcceptingStates << "\n";
}
} // namespace benchmark
