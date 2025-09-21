#include <gtest/gtest.h>
#include <map>
#include <stack>

#include "common/Stack.cpp"
#include "common/VPA.cpp"
#include "common/Word.cpp"
#include "common/transition/Transition.cpp"
#include "learner/AutomataGenerator.cpp"
#include "teacher/Converter.cpp"
#include "teacher/Teacher.cpp"
#include "teacher/cfg/Calculator.cpp"
#include "teacher/cfg/Cfg.cpp"
#include "utils/TimeMarker.cpp"

#include "learner/Learner.cpp"

using namespace common::symbol;
using namespace common::transition;

using CS = common::symbol::CallSymbol;
using LS = common::symbol::LocalSymbol;
using RS = common::symbol::ReturnSymbol;
using StackSymbol = common::symbol::StackSymbol;
using State = common::transition::State;

namespace learner
{
class TestLearner : public ::testing::Test
{
public:
    void init()
    {
        vpa = std::make_shared<VPA>(*transition, initial, acceptingStates, numOfStates);
        converter = std::make_shared<teacher::Converter>(
            vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
        oracle = std::make_shared<teacher::Teacher>(vpa, converter);

        sut = new Learner(*oracle, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
    }

    std::vector<uint16_t> acceptingStates{};
    uint16_t numOfStates{};

    State initial{0};
    CallSymbol cs{0};
    ReturnSymbol rs{0};
    LocalSymbol ls{0};
    StackSymbol ss{1};

    uint16_t numOfStackSymbols{2};
    uint16_t numOfCalls{1};
    uint16_t numOfReturns{1};
    uint16_t numOfLocals{1};

    common::Word emptyControlLetter{common::Stack{common::symbol::StackSymbol::BOTTOM}};

    std::shared_ptr<VPA> vpa;
    std::shared_ptr<Transition> transition;
    std::shared_ptr<teacher::Converter> converter;
    std::shared_ptr<teacher::Teacher> oracle;

    Learner *sut;

    bool equalUpTo(
        std::shared_ptr<VPA> hyp, const uint8_t maxLen, common::Word *counterExample = nullptr,
        common::Word pref = {})
    {
        if (vpa->checkWord(pref) != hyp->checkWord(pref))
        {
            *counterExample = pref;
            return false;
        }

        if (pref.size() == maxLen)
        {
            return true;
        }

        for (uint16_t i = 0; i < numOfCalls; ++i)
        {
            common::Word next = pref;
            next += common::Word{CallSymbol{i}};
            if (not equalUpTo(hyp, maxLen, counterExample, next))
            {
                return false;
            }
        }
        for (uint16_t i = 0; i < numOfLocals; ++i)
        {
            common::Word next = pref;
            next += common::Word{LocalSymbol{i}};
            if (not equalUpTo(hyp, maxLen, counterExample, next))
            {
                return false;
            }
        }
        for (uint16_t i = 0; i < numOfReturns; ++i)
        {
            common::Word next = pref;
            next += common::Word{ReturnSymbol{i}};
            if (not equalUpTo(hyp, maxLen, counterExample, next))
            {
                return false;
            }
        }
        return true;
    }
};

class TestLearner2 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 7;
        acceptingStates = std::vector<uint16_t>{0, 1, 2, 3, 4, 6};
        numOfStackSymbols = 5;
        numOfCalls = 3;
        numOfReturns = 1;
        numOfLocals = 2;

        transition = std::make_shared<Transition>();
        // CallTransitions:
        transition->add(State{0}, CS{0}, State{3}, StackSymbol{3});
        transition->add(State{0}, CS{1}, State{4}, StackSymbol{2});
        transition->add(State{0}, CS{2}, State{1}, StackSymbol{1});
        transition->add(State{1}, CS{0}, State{2}, StackSymbol{2});
        transition->add(State{1}, CS{1}, State{5}, StackSymbol{4});
        transition->add(State{1}, CS{2}, State{4}, StackSymbol{4});
        transition->add(State{2}, CS{0}, State{0}, StackSymbol{2});
        transition->add(State{2}, CS{1}, State{2}, StackSymbol{1});
        transition->add(State{2}, CS{2}, State{2}, StackSymbol{3});
        transition->add(State{3}, CS{0}, State{2}, StackSymbol{4});
        transition->add(State{3}, CS{1}, State{5}, StackSymbol{4});
        transition->add(State{3}, CS{2}, State{3}, StackSymbol{3});
        transition->add(State{4}, CS{0}, State{5}, StackSymbol{3});
        transition->add(State{4}, CS{1}, State{0}, StackSymbol{3});
        transition->add(State{4}, CS{2}, State{6}, StackSymbol{4});
        transition->add(State{5}, CS{0}, State{1}, StackSymbol{3});
        transition->add(State{5}, CS{1}, State{2}, StackSymbol{1});
        transition->add(State{5}, CS{2}, State{6}, StackSymbol{4});
        transition->add(State{6}, CS{0}, State{1}, StackSymbol{2});
        transition->add(State{6}, CS{1}, State{6}, StackSymbol{4});
        transition->add(State{6}, CS{2}, State{4}, StackSymbol{4});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{6});
        transition->add(State{0}, StackSymbol{2}, RS{0}, State{6});
        transition->add(State{0}, StackSymbol{3}, RS{0}, State{1});
        transition->add(State{0}, StackSymbol{4}, RS{0}, State{6});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{2}, RS{0}, State{5});
        transition->add(State{1}, StackSymbol{3}, RS{0}, State{2});
        transition->add(State{1}, StackSymbol{4}, RS{0}, State{0});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{4});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{2}, StackSymbol{2}, RS{0}, State{4});
        transition->add(State{2}, StackSymbol{3}, RS{0}, State{6});
        transition->add(State{2}, StackSymbol{4}, RS{0}, State{6});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{3}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{3}, StackSymbol{3}, RS{0}, State{2});
        transition->add(State{3}, StackSymbol{4}, RS{0}, State{2});
        transition->add(State{4}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{4}, StackSymbol{1}, RS{0}, State{6});
        transition->add(State{4}, StackSymbol{2}, RS{0}, State{3});
        transition->add(State{4}, StackSymbol{3}, RS{0}, State{0});
        transition->add(State{4}, StackSymbol{4}, RS{0}, State{5});
        transition->add(State{5}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{5}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{5}, StackSymbol{2}, RS{0}, State{2});
        transition->add(State{5}, StackSymbol{3}, RS{0}, State{2});
        transition->add(State{5}, StackSymbol{4}, RS{0}, State{6});
        transition->add(State{6}, StackSymbol{0}, RS{0}, State{2});
        transition->add(State{6}, StackSymbol{1}, RS{0}, State{6});
        transition->add(State{6}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{6}, StackSymbol{3}, RS{0}, State{6});
        transition->add(State{6}, StackSymbol{4}, RS{0}, State{5});

        // LocalTransitions:
        transition->add(State{0}, LS{0}, State{2});
        transition->add(State{0}, LS{1}, State{5});
        transition->add(State{1}, LS{0}, State{0});
        transition->add(State{1}, LS{1}, State{0});
        transition->add(State{2}, LS{0}, State{1});
        transition->add(State{2}, LS{1}, State{6});
        transition->add(State{3}, LS{0}, State{6});
        transition->add(State{3}, LS{1}, State{1});
        transition->add(State{4}, LS{0}, State{6});
        transition->add(State{4}, LS{1}, State{6});
        transition->add(State{5}, LS{0}, State{4});
        transition->add(State{5}, LS{1}, State{3});
        transition->add(State{6}, LS{0}, State{2});
        transition->add(State{6}, LS{1}, State{2});

        init();
    }
};

TEST_F(TestLearner2, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLaearner0 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 4;
        acceptingStates = std::vector<uint16_t>{0, 1};
        numOfStackSymbols = 2;
        numOfCalls = 1;
        numOfReturns = 1;
        numOfLocals = 0;

        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{3}, StackSymbol{1});
        transition->add(State{1}, CS{0}, State{2}, StackSymbol{1});
        transition->add(State{2}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{3}, CS{0}, State{2}, StackSymbol{1});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{3});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{2});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{0});

        // LocalTransitions:
        init();
    }
};

TEST_F(TestLaearner0, automaton0)
{
    std::shared_ptr<VPA> result{sut->run()};

    common::Word test{CS{0}, CS{0}, CS{0}};

    common::Word ce;
    EXPECT_TRUE(equalUpTo(result, 9, &ce)) << "counter example: " << ce;
}

class TestLaearner1 : public TestLearner
{
public:
    void SetUp() override
    {
        acceptingStates = std::vector<uint16_t>{0, 3};
        numOfStates = 5;

        transition = std::make_shared<Transition>();

        transition->add(initial, cs, s1, ss);
        transition->add(initial, ls, s1);
        transition->add(initial, StackSymbol::BOTTOM, rs, s1);

        transition->add(s1, cs, s2, ss);
        transition->add(s2, cs, s4, ss);
        transition->add(s4, cs, s4, ss);
        transition->add(s3, cs, s3, ss);

        transition->add(s1, ls, s1);
        transition->add(s2, ls, s4);
        transition->add(s4, ls, s2);
        transition->add(s3, ls, s3);

        transition->add(s1, StackSymbol::BOTTOM, rs, s1);
        transition->add(s1, ss, rs, s1);
        transition->add(s2, StackSymbol::BOTTOM, rs, s3);
        transition->add(s2, ss, rs, s1);
        transition->add(s3, StackSymbol::BOTTOM, rs, s3);
        transition->add(s3, ss, rs, s3);
        transition->add(s4, StackSymbol::BOTTOM, rs, s1);
        transition->add(s4, ss, rs, s4);

        init();
    }

    State s1{1}, s2{2}, s3{3}, s4{4};
};

TEST_F(TestLaearner1, automaton1)
{
    std::shared_ptr<VPA> result{sut->run()};

    common::Word ce;
    EXPECT_TRUE(equalUpTo(result, 9, &ce)) << "counter example: " << ce;
}

class TestLearnerBalanced : public TestLearner
{
public:
    void SetUp() override
    {
        acceptingStates = std::vector<uint16_t>{0};
        numOfStates = 3;

        transition = std::make_shared<Transition>();

        State s1{1}, dead{2};

        transition->add(initial, cs, s1, ss);
        transition->add(s1, cs, s1, ss);
        transition->add(dead, cs, dead, ss);

        transition->add(initial, ls, initial);
        transition->add(s1, ls, s1);
        transition->add(dead, ls, dead);

        transition->add(s1, StackSymbol::BOTTOM, rs, dead);
        transition->add(s1, ss, rs, initial);
        transition->add(initial, StackSymbol::BOTTOM, rs, dead);
        transition->add(dead, StackSymbol::BOTTOM, rs, dead);
        transition->add(dead, ss, rs, dead);

        init();
    }
};

TEST_F(TestLearnerBalanced, learnsBalancedUpToLen6)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce;
    common::Word test{rs, rs, rs, rs};

    EXPECT_TRUE(equalUpTo(hyp, 8, &ce)) << "counter example: " << ce;
}

class TestLearner3 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 3;
        acceptingStates = std::vector<uint16_t>{0, 1};
        numOfStackSymbols = 3;
        numOfCalls = 3;
        numOfReturns = 1;
        numOfLocals = 1;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{1}, StackSymbol{2});
        transition->add(State{0}, CS{1}, State{0}, StackSymbol{2});
        transition->add(State{0}, CS{2}, State{1}, StackSymbol{2});
        transition->add(State{1}, CS{0}, State{0}, StackSymbol{1});
        transition->add(State{1}, CS{1}, State{2}, StackSymbol{2});
        transition->add(State{1}, CS{2}, State{0}, StackSymbol{2});
        transition->add(State{2}, CS{0}, State{2}, StackSymbol{2});
        transition->add(State{2}, CS{1}, State{2}, StackSymbol{2});
        transition->add(State{2}, CS{2}, State{0}, StackSymbol{2});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{2});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{2});
        transition->add(State{0}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{2}, RS{0}, State{0});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{2}, StackSymbol{2}, RS{0}, State{1});

        // LocalTransitions:
        transition->add(State{0}, LS{0}, State{2});
        transition->add(State{1}, LS{0}, State{0});
        transition->add(State{2}, LS{0}, State{1});
        init();
    }
};

TEST_F(TestLearner3, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};
    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLearner4 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 4;
        acceptingStates = std::vector<uint16_t>{0, 1};
        numOfStackSymbols = 2;
        numOfCalls = 1;
        numOfReturns = 1;
        numOfLocals = 0;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{1}, CS{0}, State{0}, StackSymbol{1});
        transition->add(State{2}, CS{0}, State{3}, StackSymbol{1});
        transition->add(State{3}, CS{0}, State{3}, StackSymbol{1});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{3});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{2});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{0});

        // LocalTransitions:
        init();
    }
};

TEST_F(TestLearner4, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLearner5 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 4;
        acceptingStates = std::vector<uint16_t>{0, 3};
        numOfStackSymbols = 2;
        numOfCalls = 1;
        numOfReturns = 1;
        numOfLocals = 0;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{3}, StackSymbol{1});
        transition->add(State{1}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{2}, CS{0}, State{0}, StackSymbol{1});
        transition->add(State{3}, CS{0}, State{3}, StackSymbol{1});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{3});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{3});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{0});

        // LocalTransitions:
        init();
    }
};

TEST_F(TestLearner5, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLearner6 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 4;
        acceptingStates = std::vector<uint16_t>{0, 2};
        numOfStackSymbols = 2;
        numOfCalls = 1;
        numOfReturns = 1;
        numOfLocals = 0;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{1}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{2}, CS{0}, State{2}, StackSymbol{1});
        transition->add(State{3}, CS{0}, State{1}, StackSymbol{1});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{2});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{2});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{3});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{3});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{1});

        // LocalTransitions:
        init();
    }
};

TEST_F(TestLearner6, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLearner7 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 3;
        acceptingStates = std::vector<uint16_t>{0};
        numOfStackSymbols = 3;
        numOfCalls = 1;
        numOfReturns = 2;
        numOfLocals = 0;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{1}, StackSymbol{2});
        transition->add(State{1}, CS{0}, State{2}, StackSymbol{2});
        transition->add(State{2}, CS{0}, State{0}, StackSymbol{1});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{0}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{0}, StackSymbol{0}, RS{1}, State{1});
        transition->add(State{0}, StackSymbol{1}, RS{1}, State{1});
        transition->add(State{0}, StackSymbol{2}, RS{1}, State{2});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{2});
        transition->add(State{1}, StackSymbol{2}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{0}, RS{1}, State{0});
        transition->add(State{1}, StackSymbol{1}, RS{1}, State{2});
        transition->add(State{1}, StackSymbol{2}, RS{1}, State{2});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{2}, StackSymbol{2}, RS{0}, State{2});
        transition->add(State{2}, StackSymbol{0}, RS{1}, State{1});
        transition->add(State{2}, StackSymbol{1}, RS{1}, State{2});
        transition->add(State{2}, StackSymbol{2}, RS{1}, State{1});

        // LocalTransitions:
        init();
    }
};

TEST_F(TestLearner7, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLearner8 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 6;
        acceptingStates = std::vector<uint16_t>{0, 2, 3, 4};
        numOfStackSymbols = 3;
        numOfCalls = 2;
        numOfReturns = 2;
        numOfLocals = 1;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{5}, StackSymbol{1});
        transition->add(State{0}, CS{1}, State{2}, StackSymbol{2});
        transition->add(State{1}, CS{0}, State{4}, StackSymbol{2});
        transition->add(State{1}, CS{1}, State{3}, StackSymbol{2});
        transition->add(State{2}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{2}, CS{1}, State{2}, StackSymbol{1});
        transition->add(State{3}, CS{0}, State{5}, StackSymbol{2});
        transition->add(State{3}, CS{1}, State{1}, StackSymbol{2});
        transition->add(State{4}, CS{0}, State{3}, StackSymbol{2});
        transition->add(State{4}, CS{1}, State{3}, StackSymbol{2});
        transition->add(State{5}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{5}, CS{1}, State{5}, StackSymbol{1});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{4});
        transition->add(State{0}, StackSymbol{2}, RS{0}, State{4});
        transition->add(State{0}, StackSymbol{0}, RS{1}, State{3});
        transition->add(State{0}, StackSymbol{1}, RS{1}, State{1});
        transition->add(State{0}, StackSymbol{2}, RS{1}, State{2});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{2});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{2}, RS{0}, State{0});
        transition->add(State{1}, StackSymbol{0}, RS{1}, State{0});
        transition->add(State{1}, StackSymbol{1}, RS{1}, State{5});
        transition->add(State{1}, StackSymbol{2}, RS{1}, State{4});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{2}, StackSymbol{2}, RS{0}, State{3});
        transition->add(State{2}, StackSymbol{0}, RS{1}, State{2});
        transition->add(State{2}, StackSymbol{1}, RS{1}, State{3});
        transition->add(State{2}, StackSymbol{2}, RS{1}, State{3});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{3}, StackSymbol{2}, RS{0}, State{2});
        transition->add(State{3}, StackSymbol{0}, RS{1}, State{5});
        transition->add(State{3}, StackSymbol{1}, RS{1}, State{4});
        transition->add(State{3}, StackSymbol{2}, RS{1}, State{1});
        transition->add(State{4}, StackSymbol{0}, RS{0}, State{3});
        transition->add(State{4}, StackSymbol{1}, RS{0}, State{3});
        transition->add(State{4}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{4}, StackSymbol{0}, RS{1}, State{1});
        transition->add(State{4}, StackSymbol{1}, RS{1}, State{2});
        transition->add(State{4}, StackSymbol{2}, RS{1}, State{5});
        transition->add(State{5}, StackSymbol{0}, RS{0}, State{2});
        transition->add(State{5}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{5}, StackSymbol{2}, RS{0}, State{4});
        transition->add(State{5}, StackSymbol{0}, RS{1}, State{0});
        transition->add(State{5}, StackSymbol{1}, RS{1}, State{3});
        transition->add(State{5}, StackSymbol{2}, RS{1}, State{5});

        // LocalTransitions:
        transition->add(State{0}, LS{0}, State{0});
        transition->add(State{1}, LS{0}, State{3});
        transition->add(State{2}, LS{0}, State{2});
        transition->add(State{3}, LS{0}, State{1});
        transition->add(State{4}, LS{0}, State{3});
        transition->add(State{5}, LS{0}, State{0});
        init();
    }
};

TEST_F(TestLearner8, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLearner9 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 7;
        acceptingStates = std::vector<uint16_t>{0, 6};
        numOfStackSymbols = 3;
        numOfCalls = 1;
        numOfReturns = 3;
        numOfLocals = 1;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{5}, StackSymbol{2});
        transition->add(State{1}, CS{0}, State{4}, StackSymbol{2});
        transition->add(State{2}, CS{0}, State{2}, StackSymbol{1});
        transition->add(State{3}, CS{0}, State{1}, StackSymbol{1});
        transition->add(State{4}, CS{0}, State{0}, StackSymbol{1});
        transition->add(State{5}, CS{0}, State{5}, StackSymbol{2});
        transition->add(State{6}, CS{0}, State{1}, StackSymbol{2});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{3});
        transition->add(State{0}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{0}, StackSymbol{0}, RS{1}, State{5});
        transition->add(State{0}, StackSymbol{1}, RS{1}, State{1});
        transition->add(State{0}, StackSymbol{2}, RS{1}, State{2});
        transition->add(State{0}, StackSymbol{0}, RS{2}, State{1});
        transition->add(State{0}, StackSymbol{1}, RS{2}, State{6});
        transition->add(State{0}, StackSymbol{2}, RS{2}, State{1});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{2});
        transition->add(State{1}, StackSymbol{2}, RS{0}, State{3});
        transition->add(State{1}, StackSymbol{0}, RS{1}, State{4});
        transition->add(State{1}, StackSymbol{1}, RS{1}, State{4});
        transition->add(State{1}, StackSymbol{2}, RS{1}, State{4});
        transition->add(State{1}, StackSymbol{0}, RS{2}, State{6});
        transition->add(State{1}, StackSymbol{1}, RS{2}, State{2});
        transition->add(State{1}, StackSymbol{2}, RS{2}, State{2});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{5});
        transition->add(State{2}, StackSymbol{2}, RS{0}, State{5});
        transition->add(State{2}, StackSymbol{0}, RS{1}, State{2});
        transition->add(State{2}, StackSymbol{1}, RS{1}, State{4});
        transition->add(State{2}, StackSymbol{2}, RS{1}, State{2});
        transition->add(State{2}, StackSymbol{0}, RS{2}, State{2});
        transition->add(State{2}, StackSymbol{1}, RS{2}, State{2});
        transition->add(State{2}, StackSymbol{2}, RS{2}, State{3});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{6});
        transition->add(State{3}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{3}, StackSymbol{0}, RS{1}, State{2});
        transition->add(State{3}, StackSymbol{1}, RS{1}, State{2});
        transition->add(State{3}, StackSymbol{2}, RS{1}, State{1});
        transition->add(State{3}, StackSymbol{0}, RS{2}, State{6});
        transition->add(State{3}, StackSymbol{1}, RS{2}, State{0});
        transition->add(State{3}, StackSymbol{2}, RS{2}, State{3});
        transition->add(State{4}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{4}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{4}, StackSymbol{2}, RS{0}, State{1});
        transition->add(State{4}, StackSymbol{0}, RS{1}, State{0});
        transition->add(State{4}, StackSymbol{1}, RS{1}, State{4});
        transition->add(State{4}, StackSymbol{2}, RS{1}, State{6});
        transition->add(State{4}, StackSymbol{0}, RS{2}, State{2});
        transition->add(State{4}, StackSymbol{1}, RS{2}, State{4});
        transition->add(State{4}, StackSymbol{2}, RS{2}, State{2});
        transition->add(State{5}, StackSymbol{0}, RS{0}, State{1});
        transition->add(State{5}, StackSymbol{1}, RS{0}, State{5});
        transition->add(State{5}, StackSymbol{2}, RS{0}, State{0});
        transition->add(State{5}, StackSymbol{0}, RS{1}, State{4});
        transition->add(State{5}, StackSymbol{1}, RS{1}, State{6});
        transition->add(State{5}, StackSymbol{2}, RS{1}, State{5});
        transition->add(State{5}, StackSymbol{0}, RS{2}, State{0});
        transition->add(State{5}, StackSymbol{1}, RS{2}, State{6});
        transition->add(State{5}, StackSymbol{2}, RS{2}, State{3});
        transition->add(State{6}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{6}, StackSymbol{1}, RS{0}, State{6});
        transition->add(State{6}, StackSymbol{2}, RS{0}, State{3});
        transition->add(State{6}, StackSymbol{0}, RS{1}, State{4});
        transition->add(State{6}, StackSymbol{1}, RS{1}, State{4});
        transition->add(State{6}, StackSymbol{2}, RS{1}, State{5});
        transition->add(State{6}, StackSymbol{0}, RS{2}, State{2});
        transition->add(State{6}, StackSymbol{1}, RS{2}, State{2});
        transition->add(State{6}, StackSymbol{2}, RS{2}, State{0});

        // LocalTransitions:
        transition->add(State{0}, LS{0}, State{1});
        transition->add(State{1}, LS{0}, State{6});
        transition->add(State{2}, LS{0}, State{3});
        transition->add(State{3}, LS{0}, State{4});
        transition->add(State{4}, LS{0}, State{0});
        transition->add(State{5}, LS{0}, State{4});
        transition->add(State{6}, LS{0}, State{5});
        init();
    }
};

TEST_F(TestLearner9, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}

class TestLearner10 : public TestLearner
{
public:
    void SetUp() override
    {
        numOfStates = 6;
        acceptingStates = std::vector<uint16_t>{0, 2, 5};
        numOfStackSymbols = 3;
        numOfCalls = 2;
        numOfReturns = 2;
        numOfLocals = 2;
        transition = std::make_shared<Transition>();

        // CallTransitions:
        transition->add(State{0}, CS{0}, State{3}, StackSymbol{1});
        transition->add(State{0}, CS{1}, State{1}, StackSymbol{1});
        transition->add(State{1}, CS{0}, State{5}, StackSymbol{1});
        transition->add(State{1}, CS{1}, State{3}, StackSymbol{2});
        transition->add(State{2}, CS{0}, State{0}, StackSymbol{1});
        transition->add(State{2}, CS{1}, State{5}, StackSymbol{1});
        transition->add(State{3}, CS{0}, State{2}, StackSymbol{1});
        transition->add(State{3}, CS{1}, State{5}, StackSymbol{1});
        transition->add(State{4}, CS{0}, State{1}, StackSymbol{2});
        transition->add(State{4}, CS{1}, State{4}, StackSymbol{1});
        transition->add(State{5}, CS{0}, State{4}, StackSymbol{1});
        transition->add(State{5}, CS{1}, State{3}, StackSymbol{2});

        // ReturnTransitions:
        transition->add(State{0}, StackSymbol{0}, RS{0}, State{4});
        transition->add(State{0}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{0}, StackSymbol{2}, RS{0}, State{5});
        transition->add(State{0}, StackSymbol{0}, RS{1}, State{4});
        transition->add(State{0}, StackSymbol{1}, RS{1}, State{5});
        transition->add(State{0}, StackSymbol{2}, RS{1}, State{0});
        transition->add(State{1}, StackSymbol{0}, RS{0}, State{3});
        transition->add(State{1}, StackSymbol{1}, RS{0}, State{1});
        transition->add(State{1}, StackSymbol{2}, RS{0}, State{4});
        transition->add(State{1}, StackSymbol{0}, RS{1}, State{5});
        transition->add(State{1}, StackSymbol{1}, RS{1}, State{1});
        transition->add(State{1}, StackSymbol{2}, RS{1}, State{2});
        transition->add(State{2}, StackSymbol{0}, RS{0}, State{3});
        transition->add(State{2}, StackSymbol{1}, RS{0}, State{2});
        transition->add(State{2}, StackSymbol{2}, RS{0}, State{2});
        transition->add(State{2}, StackSymbol{0}, RS{1}, State{4});
        transition->add(State{2}, StackSymbol{1}, RS{1}, State{4});
        transition->add(State{2}, StackSymbol{2}, RS{1}, State{0});
        transition->add(State{3}, StackSymbol{0}, RS{0}, State{4});
        transition->add(State{3}, StackSymbol{1}, RS{0}, State{2});
        transition->add(State{3}, StackSymbol{2}, RS{0}, State{2});
        transition->add(State{3}, StackSymbol{0}, RS{1}, State{1});
        transition->add(State{3}, StackSymbol{1}, RS{1}, State{3});
        transition->add(State{3}, StackSymbol{2}, RS{1}, State{3});
        transition->add(State{4}, StackSymbol{0}, RS{0}, State{0});
        transition->add(State{4}, StackSymbol{1}, RS{0}, State{2});
        transition->add(State{4}, StackSymbol{2}, RS{0}, State{0});
        transition->add(State{4}, StackSymbol{0}, RS{1}, State{4});
        transition->add(State{4}, StackSymbol{1}, RS{1}, State{5});
        transition->add(State{4}, StackSymbol{2}, RS{1}, State{3});
        transition->add(State{5}, StackSymbol{0}, RS{0}, State{5});
        transition->add(State{5}, StackSymbol{1}, RS{0}, State{0});
        transition->add(State{5}, StackSymbol{2}, RS{0}, State{2});
        transition->add(State{5}, StackSymbol{0}, RS{1}, State{2});
        transition->add(State{5}, StackSymbol{1}, RS{1}, State{4});
        transition->add(State{5}, StackSymbol{2}, RS{1}, State{4});

        // LocalTransitions:
        transition->add(State{0}, LS{0}, State{4});
        transition->add(State{0}, LS{1}, State{4});
        transition->add(State{1}, LS{0}, State{3});
        transition->add(State{1}, LS{1}, State{0});
        transition->add(State{2}, LS{0}, State{5});
        transition->add(State{2}, LS{1}, State{0});
        transition->add(State{3}, LS{0}, State{4});
        transition->add(State{3}, LS{1}, State{2});
        transition->add(State{4}, LS{0}, State{4});
        transition->add(State{4}, LS{1}, State{4});
        transition->add(State{5}, LS{0}, State{0});
        transition->add(State{5}, LS{1}, State{1});
        init();
    }
};

TEST_F(TestLearner10, testLearner)
{
    std::shared_ptr<VPA> hyp{sut->run()};
    common::Word ce{};

    EXPECT_TRUE(equalUpTo(hyp, 7, &ce)) << "counter example: " << ce;
}
} // namespace learner
