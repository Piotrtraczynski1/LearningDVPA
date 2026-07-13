#include "benchmark/scenario/srs/IdempotencyAccuracy.hpp"
#include "Tester.hpp"
#include "generator/srs/IdempotencyGenerator.hpp"

namespace benchmark::scenario
{
std::string IdempotencyAccuracy::getGeneratorName() const
{
    return "idempotency";
}

SingleTestResult IdempotencyAccuracy::runSingle(uint32_t seed)
{
    auto result =
        Tester{1, std::make_unique<generator::srs::IdempotencyGenerator>(), parameters, seed}
            .runSingle();
    dim1Details += result.status == TestStatus::ValidationFailed;
    return result;
}

uint16_t IdempotencyAccuracy::getDim1Details()
{
    return dim1Details;
}
} // namespace benchmark::scenario
