#include "benchmark/scenario/srs/CommutationAccuracy.hpp"
#include "Tester.hpp"
#include "generator/srs/CommutativeGenerator.hpp"

namespace benchmark::scenario
{
std::string CommutationAccuracy::getGeneratorName() const
{
    return "commutative";
}

SingleTestResult CommutationAccuracy::runSingle(uint32_t seed)
{
    auto result =
        Tester{1, std::make_unique<generator::srs::CommutativeGenerator>(), parameters, seed}
            .runSingle();
    dim1Details += result.status == TestStatus::ValidationFailed;
    return result;
}

uint16_t CommutationAccuracy::getDim1Details()
{
    return dim1Details;
}
} // namespace benchmark::scenario
