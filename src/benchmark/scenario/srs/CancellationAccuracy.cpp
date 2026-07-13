#include "benchmark/scenario/srs/CancellationAccuracy.hpp"
#include "Tester.hpp"
#include "generator/srs/CancellationGenerator.hpp"

namespace benchmark::scenario
{
std::string CancellationAccuracy::getGeneratorName() const
{
    return "cancellation";
}

SingleTestResult CancellationAccuracy::runSingle(uint32_t seed)
{
    auto result =
        Tester{1, std::make_unique<generator::srs::CancellationGenerator>(), parameters, seed}
            .runSingle();
    dim1Details += result.status == TestStatus::ValidationFailed;
    return result;
}

uint16_t CancellationAccuracy::getDim1Details()
{
    return dim1Details;
}
} // namespace benchmark::scenario
