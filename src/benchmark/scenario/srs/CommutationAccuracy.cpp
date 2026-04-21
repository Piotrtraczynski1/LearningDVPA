#include "benchmark/scenario/srs/CommutationAccuracy.hpp"
#include "Tester.hpp"
#include "generator/srs/CommutativeGenerator.hpp"

namespace benchmark::scenario
{
void CommutationAccuracy::runSingleIteration()
{
    dim1Details =
        Tester{
            numOfTestsInSingleIteration,
            std::unique_ptr<generator::Generator>(new generator::srs::CommutativeGenerator()),
            parameters, rand()}
            .run();
}

uint16_t CommutationAccuracy::getDim1Details()
{
    return dim1Details;
}
} // namespace benchmark::scenario