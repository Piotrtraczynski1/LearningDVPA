#include <ctime>
#include <random>

#include "Tester.hpp"
#include "TesterParameters.hpp"
#include "generator/Generators.hpp"

#include "common/transition/Transition.hpp"

void runRandomGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::RandomGenerator()),
        RandomTestParameters};
    tester.run();
}

void runCdaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::CdaGenerator()),
        CdaTestParameters};
    tester.run();
}

void runSeVpaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::SeVpaGenerator()),
        SeVpaTestParameters};
    tester.run();
}

void runMeVpaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::MeVpaGenerator()),
        MeVpaTestParameters};
    tester.run();
}

void runECdaGenerator(const uint16_t numOfTests)
{
    Tester tester{
        numOfTests, std::unique_ptr<generator::Generator>(new generator::ECdaGenerator()),
        ECdaTestParameters};
    tester.run();
}

int main()
{
    constexpr uint16_t numOfTests{50000};

    std::cout << sizeof(common::transition::Transition<AutomatonKind::Combined>) << std::endl;

    // std::srand(std::time(0));
    std::srand(164);
    runRandomGenerator(numOfTests);
    // runECdaGenerator(numOfTests);

    return 0;
}