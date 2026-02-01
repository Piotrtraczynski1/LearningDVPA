#include <ctime>
#include <random>

#include "Tester.hpp"
#include "TesterParameters.hpp"
#include "generator/RandomGenerator.hpp"
#include "generator/XMLGenerator.hpp"
#include "generator/eCDAGenerator.hpp"

namespace
{
constexpr uint16_t numOfTests{5000};
}

void runRandomGenerator()
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::RandomGenerator()),
        RandomTestParameters};
    tester.run();
}

void runXMLGenerator()
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::XMLGenerator()),
        XMLTestParameters};
    tester.run();
}

void runeCDAGenerator()
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::eCDAGenerator()),
        eCDATestParameters};
    tester.run();
}

int main()
{
    // std::srand(std::time(0));
    std::srand(1);
    runRandomGenerator();

    return 0;
}