#include <gtest/gtest.h>
#include <memory>

#include "common/Stack.cpp"
#include "common/VPA.cpp"
#include "common/Word.cpp"
#include "common/transition/Transition.cpp"
#include "teacher/cfg/Calculator.cpp"
#include "teacher/cfg/Cfg.cpp"
#include "utils/TimeMarker.cpp"

#include "teacher/Converter.cpp"

using namespace common::transition;
using namespace common::symbol;
using namespace teacher::cfg;

namespace teacher
{

class TestConverter : public ::testing::Test
{
public:
    void SetUp() override
    {
        transition = std::make_unique<Transition<AutomatonKind::Normal>>();
        secondTransition = std::make_unique<Transition<AutomatonKind::Normal>>();
    }

    State initialState{0};

    std::unique_ptr<Transition<AutomatonKind::Normal>> transition;
    std::unique_ptr<Transition<AutomatonKind::Normal>> secondTransition;

    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa;
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis;
    std::shared_ptr<common::VPA<AutomatonKind::Combined>> combinedVpa;

    std::shared_ptr<Converter> sut;
};

TEST_F(TestConverter, combineVpaAndConvertVpaToCfg)
{
    State s1{1}, s2{2};

    CallSymbol callSymbol{0};
    LocalSymbol localSymbol{0};
    ReturnSymbol returnSymbol{0};

    StackSymbol stackSymbol{1};

    transition->add(initialState, callSymbol, s1, stackSymbol);
    transition->add(s1, callSymbol, s2, stackSymbol);
    transition->add(s2, callSymbol, s2, stackSymbol);

    secondTransition->add(initialState, callSymbol, s1, stackSymbol);
    secondTransition->add(s1, callSymbol, s2, stackSymbol);
    secondTransition->add(s2, callSymbol, s2, stackSymbol);

    transition->add(initialState, StackSymbol::BOTTOM, returnSymbol, initialState);
    transition->add(s1, StackSymbol::BOTTOM, returnSymbol, initialState);
    transition->add(s2, StackSymbol::BOTTOM, returnSymbol, s1);
    transition->add(s1, stackSymbol, returnSymbol, s1);
    transition->add(s2, stackSymbol, returnSymbol, s2);

    secondTransition->add(initialState, StackSymbol::BOTTOM, returnSymbol, initialState);
    secondTransition->add(s1, StackSymbol::BOTTOM, returnSymbol, initialState);
    secondTransition->add(s2, StackSymbol::BOTTOM, returnSymbol, s1);
    secondTransition->add(s1, stackSymbol, returnSymbol, s1);
    secondTransition->add(s2, stackSymbol, returnSymbol, s2);

    transition->add(initialState, localSymbol, initialState);
    transition->add(s1, localSymbol, s1);
    transition->add(s2, localSymbol, s2);

    secondTransition->add(initialState, localSymbol, initialState);
    secondTransition->add(s1, localSymbol, s2);
    secondTransition->add(s2, localSymbol, s1);

    vpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, std::vector<uint16_t>{0, 2}, 3);

    sut = std::make_shared<Converter>(vpa, 1, 1, 1, 2);

    hypothesis = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(secondTransition), initialState, std::vector<uint16_t>{0, 2}, 3);

    combinedVpa =
        std::make_shared<common::VPA<AutomatonKind::Combined>>(sut->combineVPA(*hypothesis));

    common::Word belongingWord{CallSymbol{0}, LocalSymbol{0}};
    common::Word bothTrue{CallSymbol{0}, CallSymbol{0}};
    common::Word bothFalse{
        CallSymbol{0}, CallSymbol{0}, ReturnSymbol{0}, ReturnSymbol{0}, ReturnSymbol{0}};

    EXPECT_TRUE(combinedVpa->checkWord(belongingWord));
    EXPECT_FALSE(combinedVpa->checkWord(bothTrue));
    EXPECT_FALSE(combinedVpa->checkWord(bothFalse));

    std::shared_ptr<cfg::Cfg> cfg{sut->convertVpaToCfg(*combinedVpa)};
    auto cfgOutput{cfg->isEmpty()};
    ASSERT_TRUE((*cfgOutput).has_value());
}

TEST_F(TestConverter, combineVpaAndConvertVpaToCfg2)
{
    State s0{0}, s1{1}, s2{2}, s3{3};

    CallSymbol c0{0};
    ReturnSymbol r0{0}, r1{1};

    StackSymbol ssB{0}, ss{1};

    transition->add(initialState, c0, s1, ss);
    transition->add(s1, c0, initialState, ss);
    transition->add(s2, c0, s3, ss);
    transition->add(s3, c0, s3, ss);

    transition->add(initialState, ssB, r0, s0);
    transition->add(initialState, ss, r1, s3);
    transition->add(s1, ssB, r0, s2);
    transition->add(s1, ss, r1, s1);
    transition->add(s2, ssB, r0, s0);
    transition->add(s2, ss, r1, s0);
    transition->add(s3, ssB, r0, s0);
    transition->add(s3, ss, r1, s0);

    secondTransition->add(initialState, c0, initialState, ss);
    secondTransition->add(initialState, ssB, r0, initialState);

    vpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, std::vector<uint16_t>{0, 1}, 4);
    hypothesis = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(secondTransition), initialState, std::vector<uint16_t>{0}, 1);

    sut = std::make_shared<Converter>(vpa, 1, 2, 0, 2);

    combinedVpa =
        std::make_shared<common::VPA<AutomatonKind::Combined>>(sut->combineVPA(*hypothesis));

    common::Word expectTrue{c0, c0, c0, c0, c0, r1};

    EXPECT_TRUE(combinedVpa->checkWord(expectTrue));

    std::shared_ptr<cfg::Cfg> cfg{sut->convertVpaToCfg(*combinedVpa)};
    auto cfgOutput{cfg->isEmpty()};
    ASSERT_TRUE((*cfgOutput).has_value());
}

TEST_F(TestConverter, combineVpaAndConvertVpaToCfg3)
{
    using CS = common::symbol::CallSymbol;
    using RS = common::symbol::ReturnSymbol;

    uint16_t numOfStates = 4;
    auto acceptingStates = std::vector<uint16_t>{0, 1};
    uint16_t numOfStackSymbols = 2;
    uint16_t numOfCalls = 1;
    uint16_t numOfReturns = 1;
    uint16_t numOfLocals = 0;

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

    // CallTransitions:
    secondTransition->add(State{0}, CS{0}, State{1}, StackSymbol{1});
    secondTransition->add(State{1}, CS{0}, State{2}, StackSymbol{1});
    secondTransition->add(State{2}, CS{0}, State{0}, StackSymbol{1});

    // ReturnTransitions:
    secondTransition->add(State{0}, StackSymbol{0}, RS{0}, State{0});
    secondTransition->add(State{0}, StackSymbol{1}, RS{0}, State{0});
    secondTransition->add(State{1}, StackSymbol{0}, RS{0}, State{1});
    secondTransition->add(State{1}, StackSymbol{1}, RS{0}, State{0});
    secondTransition->add(State{2}, StackSymbol{0}, RS{0}, State{0});
    secondTransition->add(State{2}, StackSymbol{1}, RS{0}, State{0});

    vpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, acceptingStates, numOfStates);
    hypothesis = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(secondTransition), initialState, std::vector<uint16_t>{0}, 3);

    sut = std::make_shared<Converter>(vpa, 1, 1, 0, 2);

    combinedVpa =
        std::make_shared<common::VPA<AutomatonKind::Combined>>(sut->combineVPA(*hypothesis));

    common::Word expectFalse{CS{0}, RS{0}, RS{0}, RS{0}};

    EXPECT_TRUE(vpa->checkWord(expectFalse));
    EXPECT_TRUE(hypothesis->checkWord(expectFalse));
    EXPECT_FALSE(combinedVpa->checkWord(expectFalse));

    std::shared_ptr<cfg::Cfg> cfg{sut->convertVpaToCfg(*combinedVpa)};
    auto cfgOutput{cfg->isEmpty()};
    ASSERT_TRUE((*cfgOutput).has_value());
}

TEST_F(TestConverter, combineVpaAndConvertVpaToCfg4)
{
    using CS = common::symbol::CallSymbol;
    using RS = common::symbol::ReturnSymbol;
    using LS = common::symbol::LocalSymbol;

    uint16_t numOfStates = 5;
    auto acceptingStates = std::vector<uint16_t>{0, 3};
    uint16_t numOfStackSymbols = 2;
    uint16_t numOfCalls = 1;
    uint16_t numOfReturns = 1;
    uint16_t numOfLocals = 1;

    // CallTransitions:
    transition->add(State{0}, CS{0}, State{1}, StackSymbol{1});
    transition->add(State{1}, CS{0}, State{2}, StackSymbol{1});
    transition->add(State{2}, CS{0}, State{4}, StackSymbol{1});
    transition->add(State{3}, CS{0}, State{3}, StackSymbol{1});
    transition->add(State{4}, CS{0}, State{4}, StackSymbol{1});

    // ReturnTransitions:
    transition->add(State{0}, StackSymbol{0}, RS{0}, State{1});
    transition->add(State{1}, StackSymbol{0}, RS{0}, State{1});
    transition->add(State{1}, StackSymbol{1}, RS{0}, State{1});
    transition->add(State{2}, StackSymbol{0}, RS{0}, State{3});
    transition->add(State{2}, StackSymbol{1}, RS{0}, State{1});
    transition->add(State{3}, StackSymbol{0}, RS{0}, State{3});
    transition->add(State{3}, StackSymbol{1}, RS{0}, State{3});
    transition->add(State{4}, StackSymbol{0}, RS{0}, State{1});
    transition->add(State{4}, StackSymbol{1}, RS{0}, State{4});

    // LocalTransitions:
    transition->add(State{0}, LS{0}, State{1});
    transition->add(State{1}, LS{0}, State{1});
    transition->add(State{2}, LS{0}, State{4});
    transition->add(State{3}, LS{0}, State{3});
    transition->add(State{4}, LS{0}, State{2});

    // CallTransitions:
    secondTransition->add(State{0}, CS{0}, State{1}, StackSymbol{1});
    secondTransition->add(State{1}, CS{0}, State{2}, StackSymbol{1});
    secondTransition->add(State{2}, CS{0}, State{2}, StackSymbol{1});

    // ReturnTransitions:
    secondTransition->add(State{0}, StackSymbol{0}, RS{0}, State{1});
    secondTransition->add(State{0}, StackSymbol{1}, RS{0}, State{1});
    secondTransition->add(State{1}, StackSymbol{0}, RS{0}, State{1});
    secondTransition->add(State{1}, StackSymbol{1}, RS{0}, State{1});
    secondTransition->add(State{2}, StackSymbol{0}, RS{0}, State{0});
    secondTransition->add(State{2}, StackSymbol{1}, RS{0}, State{1});

    // LocalTransitions:
    secondTransition->add(State{0}, LS{0}, State{1});
    secondTransition->add(State{1}, LS{0}, State{1});
    secondTransition->add(State{2}, LS{0}, State{2});

    vpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, acceptingStates, numOfStates);
    hypothesis = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(secondTransition), initialState, std::vector<uint16_t>{0}, 3);

    sut =
        std::make_shared<Converter>(vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

    combinedVpa =
        std::make_shared<common::VPA<AutomatonKind::Combined>>(sut->combineVPA(*hypothesis));

    common::Word expectTrue{CS{0}, CS{0}, CS{0}, RS{0}, RS{0}, RS{0}, LS{0}, RS{0}};

    EXPECT_TRUE(vpa->checkWord(expectTrue));
    EXPECT_FALSE(hypothesis->checkWord(expectTrue));
    EXPECT_TRUE(combinedVpa->checkWord(expectTrue));

    std::shared_ptr<cfg::Cfg> cfg{sut->convertVpaToCfg(*combinedVpa)};
    auto cfgOutput{cfg->isEmpty()};
    ASSERT_TRUE((*cfgOutput).has_value());
}
} // namespace teacher
