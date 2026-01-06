#include <benchmark/benchmark.h>
#include <cstdint>
#include <random>

#include "Tester.hpp"
#include "TesterParameters.hpp"
#include "generator/RandomGenerator.hpp"
#include "generator/XMLGenerator.hpp"
#include "utils/TimeMarker.hpp"

static void runRandomGenerator()
{
    Tester tester{
        5, std::unique_ptr<generator::Generator>(new generator::RandomGenerator()),
        RandomTestParameters};
    tester.run();
}

static void test(benchmark::State &state)
{
    std::srand(1);
    while (state.KeepRunning())
    {
        MeasurementDataBase::reset();

        runRandomGenerator();

        auto hce = MeasurementDataBase::getMarkerInfo("handleCounterExample");
        auto gen = MeasurementDataBase::getMarkerInfo("generateAutomaton");

        state.counters["handleCE_us"] = hce.time;
        state.counters["handleCE_calls"] = hce.executions;
        state.counters["handleCE_avg_us"] = hce.time / hce.executions;

        state.counters["genAuto_us"] = gen.time;
        state.counters["genAuto_calls"] = gen.executions;
        state.counters["genAuto_avg_us"] = gen.time / gen.executions;
    }
}

BENCHMARK(test);

BENCHMARK_MAIN();
