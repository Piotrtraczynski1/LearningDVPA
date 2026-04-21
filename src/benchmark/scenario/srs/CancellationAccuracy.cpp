#include "benchmark/scenario/srs/CancellationAccuracy.hpp"
#include "Tester.hpp"
#include "generator/srs/CancellationGenerator.hpp"

namespace benchmark::scenario
{
void CancellationAccuracy::runSingleIteration()
{
    dim1Details =
        Tester{
            numOfTestsInSingleIteration,
            std::unique_ptr<generator::Generator>(new generator::srs::CancellationGenerator()),
            parameters, rand()}
            .run();
}

uint16_t CancellationAccuracy::getDim1Details()
{
    return dim1Details;
}
} // namespace benchmark::scenario