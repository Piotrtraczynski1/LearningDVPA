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
TEST(Converter, convertVpaToCfg)
{
    State initialState{0};
    State s2{1};

    std::shared_ptr<Transition> transition = std::make_shared<Transition>();

    transition->add(initialState, CallSymbol{0}, s2, StackSymbol{1});
    transition->add(s2, StackSymbol{1}, ReturnSymbol{1}, initialState);

    common::VPA vpa{*transition, initialState, {0}, 2};

    Converter sut{std::make_shared<common::VPA>(vpa), 1, 1, 0, 2};
    std::shared_ptr<cfg::Cfg> cfg{sut.convertVpaToCfg(vpa)};

    auto cfgOutput{cfg->isEmpty()};

    ASSERT_TRUE((*cfgOutput).has_value());
}

TEST(Converter, convertVpaToCfgV2)
{
    State initialState{0};
    State s1{1};

    CallSymbol callSymbol{0};
    LocalSymbol localSymbol_0{0};
    LocalSymbol localSymbol_1{1};
    ReturnSymbol returnSymbol{0};

    StackSymbol stackSymbol_1{1};
    StackSymbol stackSymbol_2{2};

    std::shared_ptr<Transition> transition = std::make_shared<Transition>();

    transition->add(initialState, callSymbol, s1, stackSymbol_1);
    transition->add(s1, callSymbol, s1, stackSymbol_2);

    transition->add(initialState, localSymbol_0, initialState);
    transition->add(s1, localSymbol_0, s1);

    transition->add(initialState, localSymbol_1, initialState);
    transition->add(s1, localSymbol_1, s1);

    transition->add(s1, stackSymbol_1, returnSymbol, initialState);
    transition->add(s1, stackSymbol_2, returnSymbol, s1);

    common::VPA vpa{*transition, initialState, {0}, 2};

    Converter sut{std::make_shared<common::VPA>(vpa), 1, 1, 2, 3};

    std::shared_ptr<cfg::Cfg> cfg{sut.convertVpaToCfg(vpa)};

    auto cfgOutput{cfg->isEmpty()};

    ASSERT_TRUE((*cfgOutput).has_value());
}

TEST(Converter, combineVpa)
{
    State initialState{0};
    State s1{1}, s2{2};

    CallSymbol callSymbol{0};
    LocalSymbol localSymbol{0};
    ReturnSymbol returnSymbol{0};

    StackSymbol stackSymbol{1};

    std::shared_ptr<Transition> transition = std::make_shared<Transition>();
    std::shared_ptr<Transition> secondTransition = std::make_shared<Transition>();

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

    common::VPA vpa{*transition, initialState, {0, 2}, 3};

    Converter sut{std::make_shared<common::VPA>(vpa), 1, 1, 1, 2};

    common::VPA hypothesis{*secondTransition, initialState, {0, 2}, 3};

    common::VPA combinedVpa{sut.combineVPA(hypothesis)};

    common::Word belongingWord{CallSymbol{0}, LocalSymbol{0}};
    common::Word bothTrue{CallSymbol{0}, CallSymbol{0}};
    common::Word bothFalse{
        CallSymbol{0}, CallSymbol{0}, ReturnSymbol{0}, ReturnSymbol{0}, ReturnSymbol{0}};

    EXPECT_TRUE(combinedVpa.checkWord(belongingWord));
    EXPECT_FALSE(combinedVpa.checkWord(bothTrue));
    EXPECT_FALSE(combinedVpa.checkWord(bothFalse));
}
} // namespace teacher
