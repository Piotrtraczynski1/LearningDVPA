#include <gtest/gtest.h>

#include "common/Stack.cpp"
#include "common/VPA.cpp"
#include "common/Word.cpp"
#include "common/transition/Transition.cpp"
#include "teacher/Converter.cpp"
#include "teacher/Teacher.cpp"
#include "teacher/cfg/Calculator.cpp"
#include "teacher/cfg/Cfg.cpp"
#include "utils/TimeMarker.cpp"

#include "learner/AutomataGenerator.cpp"

using namespace common::symbol;
using namespace common::transition;

constexpr uint16_t numOfCalls{1};
constexpr uint16_t numOfReturns{1};
constexpr uint16_t numOfLocals{1};
constexpr uint16_t numOfStackSymbols{2};

namespace learner
{
class TestAutomataGenerator : public ::testing::Test
{
public:
    void init()
    {
        vpa = std::make_shared<VPA>(transition, initial, acceptingStates, numOfStates);
        converter = std::make_shared<teacher::Converter>(
            vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
        oracle = std::make_shared<teacher::Teacher>(vpa, converter);

        selectors = std::make_shared<Selectors>();
        selectors->addSelector(common::Word{}, true);
        testWords = std::make_shared<TestWords>();

        sut = new AutomataGenerator(
            *oracle, selectors, testWords, numOfCalls, numOfReturns, numOfLocals,
            numOfStackSymbols);
    }

    std::vector<uint16_t> acceptingStates{};
    uint16_t numOfStates{};

    State initial{0};
    CallSymbol cs{0};
    ReturnSymbol rs{0};
    LocalSymbol ls{0};
    StackSymbol ss{1};

    common::Word emptyControlLetter{common::Stack{common::symbol::StackSymbol::BOTTOM}};

    std::shared_ptr<VPA> vpa;
    Transition transition;
    std::shared_ptr<teacher::Converter> converter;
    std::shared_ptr<teacher::Teacher> oracle;

    std::shared_ptr<Selectors> selectors;
    std::shared_ptr<TestWords> testWords;

    AutomataGenerator *sut;
};

class TestAutomataGeneratorFourStatesReducedToTwo : public TestAutomataGenerator
{
public:
    void SetUp() override
    {
        numOfStates = 4;
        acceptingStates = std::vector<uint16_t>{0, 2};

        transition = Transition{};

        transition.add(initial, cs, s1, ss);
        transition.add(s1, cs, s2, ss);
        transition.add(s2, cs, s3, ss);
        transition.add(s3, cs, initial, ss);

        transition.add(initial, ls, s1);
        transition.add(s1, ls, s2);
        transition.add(s2, ls, s3);
        transition.add(s3, ls, initial);

        transition.add(initial, StackSymbol::BOTTOM, rs, s1);
        transition.add(s1, StackSymbol::BOTTOM, rs, s2);
        transition.add(s2, StackSymbol::BOTTOM, rs, s3);
        transition.add(s3, StackSymbol::BOTTOM, rs, initial);

        transition.add(initial, ss, rs, s1);
        transition.add(s1, ss, rs, s2);
        transition.add(s2, ss, rs, s3);
        transition.add(s3, ss, rs, initial);

        init();
    }

    State s1{1}, s2{2}, s3{3};
};

TEST_F(TestAutomataGeneratorFourStatesReducedToTwo, automataSimulationStep1)
{
    std::shared_ptr<VPA> automata{sut->generate()};

    EXPECT_EQ(automata->acceptingStates.at(0), true);
    EXPECT_EQ(automata->acceptingStates.at(1), false);

    EXPECT_EQ(*oracle->equivalenceQuery(automata), common::Word{});
}

class TestAutomataGenerator1 : public TestAutomataGenerator
{
public:
    void SetUp() override
    {
        acceptingStates = std::vector<uint16_t>{0, 3};
        numOfStates = 5;

        transition = Transition{};

        transition.add(initial, cs, s1, ss);
        transition.add(initial, ls, s1);
        transition.add(initial, StackSymbol::BOTTOM, rs, s1);

        transition.add(s1, cs, s2, ss);
        transition.add(s2, cs, s4, ss);
        transition.add(s4, cs, s4, ss);
        transition.add(s3, cs, s3, ss);

        transition.add(s1, ls, s1);
        transition.add(s2, ls, s4);
        transition.add(s4, ls, s2);
        transition.add(s3, ls, s3);

        transition.add(s1, StackSymbol::BOTTOM, rs, s1);
        transition.add(s1, ss, rs, s1);
        transition.add(s2, StackSymbol::BOTTOM, rs, s3);
        transition.add(s2, ss, rs, s1);
        transition.add(s3, StackSymbol::BOTTOM, rs, s3);
        transition.add(s3, ss, rs, s3);
        transition.add(s4, StackSymbol::BOTTOM, rs, s1);
        transition.add(s4, ss, rs, s4);

        init();
    }

    State s1{1}, s2{2}, s3{3}, s4{4};
};

TEST_F(TestAutomataGenerator1, automataSimulationStep1)
{
    std::shared_ptr<VPA> automata{sut->generate()};

    common::Word counterExample{*oracle->equivalenceQuery(automata)};

    EXPECT_EQ(automata->acceptingStates.at(0), true);
    EXPECT_EQ(automata->acceptingStates.at(1), false);

    EXPECT_EQ(automata->checkWord(common::Word{rs}), false);
    EXPECT_EQ(automata->checkWord(common::Word{ls}), false);
    EXPECT_EQ(automata->checkWord(common::Word{cs}), false);

    common::Word expectedCounterExample{cs, cs, cs, rs, rs, rs, ls, rs};
    EXPECT_EQ(counterExample, expectedCounterExample);
    EXPECT_NE(automata->checkWord(counterExample), vpa->checkWord(counterExample));
}

TEST_F(TestAutomataGenerator1, automataSimulationStep2)
{
    selectors->addSelector({cs}, false);
    selectors->addSelector({cs, rs, cs, cs, rs, rs, ls, rs}, true);

    std::shared_ptr<VPA> automata{sut->generate()};

    common::Word counterExample{*oracle->equivalenceQuery(automata)};

    EXPECT_EQ(automata->acceptingStates.at(0), true);
    EXPECT_EQ(automata->acceptingStates.at(1), false);

    EXPECT_EQ(automata->checkWord(common::Word{rs}), false);
    EXPECT_EQ(automata->checkWord(common::Word{ls}), false);
    EXPECT_EQ(automata->checkWord(common::Word{cs}), false);

    common::Word expectedCounterExample{cs, cs, cs, rs, rs, rs, ls, rs};

    EXPECT_EQ(counterExample, expectedCounterExample);
    EXPECT_NE(automata->checkWord(counterExample), vpa->checkWord(counterExample));
}
} // namespace learner
