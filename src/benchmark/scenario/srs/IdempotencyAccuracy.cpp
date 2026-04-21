#include "benchmark/scenario/srs/IdempotencyAccuracy.hpp"
#include "Tester.hpp"
#include "generator/srs/IdempotencyGenerator.hpp"

namespace benchmark::scenario
{
void IdempotencyAccuracy::runSingleIteration()
{
    dim1Details =
        Tester{
            numOfTestsInSingleIteration,
            std::unique_ptr<generator::Generator>(new generator::srs::IdempotencyGenerator()),
            parameters, rand()}
            .run();
}

uint16_t IdempotencyAccuracy::getDim1Details()
{
    return dim1Details;
}
} // namespace benchmark::scenario