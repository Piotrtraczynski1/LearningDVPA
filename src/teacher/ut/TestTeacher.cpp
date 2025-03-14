#include <gtest/gtest.h>
#include <memory>

#include "common/Stack.cpp"
#include "common/VPA.cpp"
#include "common/Word.cpp"
#include "common/transition/Transition.cpp"
#include "teacher/Converter.cpp"
#include "teacher/cfg/Calculator.cpp"
#include "teacher/cfg/Cfg.cpp"
#include "utils/TimeMarker.cpp"

#include "teacher/Teacher.cpp"

using namespace common::transition;
using namespace common::symbol;
using namespace teacher::cfg;

namespace teacher
{
using CS = common::symbol::CallSymbol;
using RS = common::symbol::ReturnSymbol;
using LS = common::symbol::LocalSymbol;
using SS = common::symbol::StackSymbol;

class TestTeacherSimple : public ::testing::Test
{
public:
    State initialState{0};
    State s1{1};
    State s2{2};

    std::shared_ptr<Transition> transitionOne;
    std::shared_ptr<Transition> transitionTwo;

    std::shared_ptr<Converter> converter;

    std::shared_ptr<Teacher> sut;

    void SetUp() override
    {
        transitionOne = std::make_shared<Transition>();
        transitionTwo = std::make_shared<Transition>();

        transitionOne->add(initialState, LS{0}, s1);
        transitionOne->add(s1, LS{0}, s1);

        transitionTwo->add(initialState, LS{0}, s1);
        transitionTwo->add(s1, LS{0}, s2);
        transitionTwo->add(s2, LS{0}, s2);
    }

    void init(
        std::shared_ptr<VPA> vpa, uint16_t numCalls, uint16_t numReturns, uint16_t numLocals,
        uint16_t numOfStackSymbols)
    {
        converter =
            std::make_shared<Converter>(vpa, numCalls, numReturns, numLocals, numOfStackSymbols);
        sut = std::make_shared<Teacher>(vpa, converter);
    }

    std::shared_ptr<common::Word> equivalenceQuery(common::VPA &secondVpa, Converter &sut)
    {
        common::VPA combinedVpa{sut.combineVPA(secondVpa)};
        std::shared_ptr<cfg::Cfg> cfg{sut.convertVpaToCfg(combinedVpa)};

        auto cfgOutput{cfg->isEmpty()};
        return cfg::Calculator::convertCfgOutputToWord(*cfgOutput);
    }
};

TEST_F(TestTeacherSimple, equivalenceQuery)
{
    std::shared_ptr<common::VPA> vpa =
        std::make_shared<common::VPA>(*transitionOne, initialState, std::vector<uint16_t>{1}, 2);
    std::shared_ptr<common::VPA> hypothesis =
        std::make_shared<common::VPA>(*transitionTwo, initialState, std::vector<uint16_t>{2}, 3);

    init(vpa, 0, 0, 1, 1);

    common::Word res{*sut->equivalenceQuery(hypothesis)};
    common::Word expectedWord{LS{0}};
    EXPECT_EQ(res, expectedWord);
}

TEST_F(TestTeacherSimple, equivalenceQuery2)
{
    std::shared_ptr<common::VPA> vpa =
        std::make_shared<common::VPA>(*transitionOne, initialState, std::vector<uint16_t>{1}, 2);

    init(vpa, 0, 0, 1, 1);

    common::Word res{*sut->equivalenceQuery(vpa)};
    common::Word expectedWord{};
    EXPECT_EQ(res, expectedWord);
}

TEST_F(TestTeacherSimple, equivalenceQuer3)
{
    std::shared_ptr<common::VPA> vpa =
        std::make_shared<common::VPA>(*transitionTwo, initialState, std::vector<uint16_t>{2}, 3);
    init(vpa, 0, 0, 1, 1);

    common::Word res{*sut->equivalenceQuery(vpa)};
    common::Word expectedWord{};
    EXPECT_EQ(res, expectedWord);
}

class TestTeacher : public ::testing::Test
{
public:
    State initial{0};
    uint16_t numOfCalls{}, numOfReturns{}, numOfLocals{}, numOfStackSymbols{};

    CS cs{0};
    RS rs{0};
    LS ls{0};
    common::symbol::StackSymbol ss{1};

    std::shared_ptr<Transition> vpaTransition;
    std::shared_ptr<common::VPA> vpa;
    std::shared_ptr<Converter> converter;

    std::shared_ptr<Teacher> sut;

    std::shared_ptr<Transition> hypothesisTransition;
    std::shared_ptr<common::VPA> hypothesis;

    void SetUp() override
    {
        vpaTransition = std::make_shared<Transition>();
        hypothesisTransition = std::make_shared<Transition>();
    }

    void init(const uint16_t c, const uint16_t r, const uint16_t l, const uint16_t ss)
    {
        numOfCalls = c;
        numOfReturns = r;
        numOfLocals = l;
        numOfStackSymbols = ss;
    }

    void initSut(const std::vector<uint16_t> &acceptingStates, const uint16_t numOfStates)
    {
        vpa = std::make_shared<common::VPA>(*vpaTransition, initial, acceptingStates, numOfStates);
        converter = std::make_shared<Converter>(
            vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

        sut = std::make_shared<Teacher>(vpa, converter);
    }

    void initHypothesis(const std::vector<uint16_t> &acceptingStates, const uint16_t numOfStates)
    {
        hypothesis = std::make_shared<common::VPA>(
            *hypothesisTransition, initial, acceptingStates, numOfStates);
    }
};

TEST_F(TestTeacher, testMatchCallsReturns)
{
    constexpr uint16_t numOfCalls{1}, numOfReturns{1}, numOfLocals{1}, numOfStackSymbols{2};
    init(numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

    std::vector<uint16_t> acceptingStates = std::vector<uint16_t>{initial};
    uint16_t numOfStates{3};
    State s1{1}, dead{2};

    vpaTransition->add(initial, cs, s1, ss);
    vpaTransition->add(s1, cs, s1, ss);
    vpaTransition->add(dead, cs, dead, ss);

    vpaTransition->add(s1, ss, rs, initial);
    vpaTransition->add(s1, StackSymbol::BOTTOM, rs, dead);
    vpaTransition->add(initial, StackSymbol::BOTTOM, rs, dead);
    vpaTransition->add(initial, ss, rs, dead);
    vpaTransition->add(dead, StackSymbol::BOTTOM, rs, dead);
    vpaTransition->add(dead, ss, rs, dead);

    vpaTransition->add(initial, ls, initial);
    vpaTransition->add(s1, ls, s1);
    vpaTransition->add(dead, ls, dead);

    initSut(acceptingStates, numOfStates);

    hypothesisTransition->add(initial, cs, s1, ss);
    hypothesisTransition->add(s1, cs, s1, ss);
    hypothesisTransition->add(dead, cs, dead, ss);

    hypothesisTransition->add(initial, ls, initial);
    hypothesisTransition->add(s1, ls, s1);
    hypothesisTransition->add(dead, ls, dead);

    hypothesisTransition->add(initial, StackSymbol::BOTTOM, rs, s1);
    hypothesisTransition->add(initial, ss, rs, dead);
    hypothesisTransition->add(s1, StackSymbol::BOTTOM, rs, s1);
    hypothesisTransition->add(s1, ss, rs, initial);
    hypothesisTransition->add(dead, StackSymbol::BOTTOM, rs, dead);
    hypothesisTransition->add(dead, ss, rs, dead);

    initHypothesis(acceptingStates, numOfStates);

    common::Word tmp{rs, cs, rs};

    common::Word counterExample{*sut->equivalenceQuery(hypothesis)};

    EXPECT_NE(counterExample, common::Word{}); // [C-0, L-0, R-0, R-0, C-0, R-0]
    EXPECT_NE(vpa->checkWord(counterExample), hypothesis->checkWord(counterExample));
}

TEST_F(TestTeacher, testMatchCallsReturns2)
{
    constexpr uint16_t numOfCalls{1}, numOfReturns{1}, numOfLocals{1}, numOfStackSymbols{2};
    init(numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

    std::vector<uint16_t> acceptingStates = std::vector<uint16_t>{initial, 3};
    uint16_t numOfStates{5};
    State s1{1}, s2{2}, s3{3}, s4{4};

    vpaTransition->add(initial, cs, s1, ss);
    vpaTransition->add(s1, cs, s2, ss);
    vpaTransition->add(s2, cs, s4, ss);
    vpaTransition->add(s3, cs, s3, ss);
    vpaTransition->add(s4, cs, s4, ss);

    vpaTransition->add(initial, StackSymbol::BOTTOM, rs, s1);
    vpaTransition->add(initial, ss, rs, s1);
    vpaTransition->add(s1, StackSymbol::BOTTOM, rs, s1);
    vpaTransition->add(s1, ss, rs, s1);
    vpaTransition->add(s2, StackSymbol::BOTTOM, rs, s3);
    vpaTransition->add(s2, ss, rs, s1);
    vpaTransition->add(s3, StackSymbol::BOTTOM, rs, s3);
    vpaTransition->add(s3, ss, rs, s3);
    vpaTransition->add(s4, StackSymbol::BOTTOM, rs, s1);
    vpaTransition->add(s4, ss, rs, s4);

    vpaTransition->add(initial, ls, s1);
    vpaTransition->add(s1, ls, s1);
    vpaTransition->add(s2, ls, s4);
    vpaTransition->add(s3, ls, s3);
    vpaTransition->add(s4, ls, s2);

    initSut(acceptingStates, numOfStates);

    std::vector<uint16_t> hypothesisAcceptingStates = std::vector<uint16_t>{initial};
    uint16_t hypothesisNumOfStates{4};

    hypothesisTransition->add(initial, cs, s1, ss);
    hypothesisTransition->add(s1, cs, s2, ss);
    hypothesisTransition->add(s2, cs, s3, ss);
    hypothesisTransition->add(s3, cs, s3, ss);

    hypothesisTransition->add(initial, StackSymbol::BOTTOM, rs, s1);
    hypothesisTransition->add(initial, ss, rs, s1);
    hypothesisTransition->add(s1, StackSymbol::BOTTOM, rs, s1);
    hypothesisTransition->add(s1, ss, rs, s1);
    hypothesisTransition->add(s2, StackSymbol::BOTTOM, rs, initial);
    hypothesisTransition->add(s2, ss, rs, s1);
    hypothesisTransition->add(s3, StackSymbol::BOTTOM, rs, s1);
    hypothesisTransition->add(s3, ss, rs, s3);

    hypothesisTransition->add(initial, ls, s1);
    hypothesisTransition->add(s1, ls, s1);
    hypothesisTransition->add(s2, ls, s3);
    hypothesisTransition->add(s3, ls, s2);

    initHypothesis(hypothesisAcceptingStates, hypothesisNumOfStates);

    common::Word counterExample{*sut->equivalenceQuery(hypothesis)};

    EXPECT_NE(counterExample, common::Word{});
    EXPECT_NE(vpa->checkWord(counterExample), hypothesis->checkWord(counterExample));
}

TEST_F(TestTeacher, testStackContentQuery)
{
    constexpr uint16_t numOfCalls{2}, numOfReturns{0}, numOfLocals{0}, numOfStackSymbols{3};
    init(numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

    uint16_t numOfStates{1};
    std::vector<uint16_t> acceptingStates = std::vector<uint16_t>{initial};

    vpaTransition->add(initial, CS{0}, initial, SS{1});
    vpaTransition->add(initial, CS{1}, initial, SS{2});

    initSut(acceptingStates, numOfStates);

    common::Word word{CS{0}, CS{1}, CS{1}, CS{0}, CS{0}};
    common::Stack expectedStack{SS{1}, SS{2}, SS{2}, SS{1}, SS{1}};

    EXPECT_EQ(expectedStack, sut->stackContentQuery(word));
}
} // namespace teacher
