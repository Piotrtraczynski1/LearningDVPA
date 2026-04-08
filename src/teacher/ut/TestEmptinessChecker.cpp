#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"
#include "teacher/EmptinessChecker.hpp"

using namespace testing;
using common::symbol::CallSymbol;
using common::symbol::LocalSymbol;
using common::symbol::ReturnSymbol;
using common::symbol::StackSymbol;
using common::transition::State;

namespace
{
constexpr uint16_t numOfCalls{2};
constexpr uint16_t numOfReturns{1};
constexpr uint16_t numOfLocals{3};
constexpr uint16_t numOfStackSymbols{3};
constexpr State initialState{0};
constexpr uint16_t numOfStates{5};
} // namespace

namespace teacher
{
class TestEmptinessChecker : public Test
{
public:
    void SetUp() override
    {
        initTransition();
    }

    void initTransition()
    {
        transition = std::make_unique<common::transition::Transition<AutomatonKind::Combined>>();

        // locals
        transition->add(State{0}, LocalSymbol{0}, State{1});
        transition->add(State{1}, LocalSymbol{0}, State{2});
        transition->add(State{2}, LocalSymbol{0}, State{3});
        transition->add(State{3}, LocalSymbol{0}, State{4});
        transition->add(State{4}, LocalSymbol{0}, State{0});

        transition->add(State{0}, LocalSymbol{1}, State{2});
        transition->add(State{1}, LocalSymbol{1}, State{1});
        transition->add(State{2}, LocalSymbol{1}, State{4});
        transition->add(State{3}, LocalSymbol{1}, State{4});
        transition->add(State{4}, LocalSymbol{1}, State{3});

        transition->add(State{0}, LocalSymbol{2}, State{4});
        transition->add(State{1}, LocalSymbol{2}, State{3});
        transition->add(State{2}, LocalSymbol{2}, State{2});
        transition->add(State{3}, LocalSymbol{2}, State{1});
        transition->add(State{4}, LocalSymbol{2}, State{1});

        // returns
        transition->add(State{0}, StackSymbol{0}, ReturnSymbol{0}, State{2});
        transition->add(State{1}, StackSymbol{0}, ReturnSymbol{0}, State{3});
        transition->add(State{2}, StackSymbol{0}, ReturnSymbol{0}, State{4});
        transition->add(State{3}, StackSymbol{0}, ReturnSymbol{0}, State{2});
        transition->add(State{4}, StackSymbol{0}, ReturnSymbol{0}, State{1});

        transition->add(State{0}, StackSymbol{1}, ReturnSymbol{0}, State{1});
        transition->add(State{1}, StackSymbol{1}, ReturnSymbol{0}, State{3});
        transition->add(State{2}, StackSymbol{1}, ReturnSymbol{0}, State{3});
        transition->add(State{3}, StackSymbol{1}, ReturnSymbol{0}, State{4});
        transition->add(State{4}, StackSymbol{1}, ReturnSymbol{0}, State{2});

        transition->add(State{0}, StackSymbol{2}, ReturnSymbol{0}, State{2});
        transition->add(State{1}, StackSymbol{2}, ReturnSymbol{0}, State{3});
        transition->add(State{2}, StackSymbol{2}, ReturnSymbol{0}, State{2});
        transition->add(State{3}, StackSymbol{2}, ReturnSymbol{0}, State{4});
        transition->add(State{4}, StackSymbol{2}, ReturnSymbol{0}, State{4});

        // calls
        transition->add(State{0}, CallSymbol{0}, State{4}, StackSymbol{2});
        transition->add(State{1}, CallSymbol{0}, State{3}, StackSymbol{1});
        transition->add(State{2}, CallSymbol{0}, State{2}, StackSymbol{2});
        transition->add(State{3}, CallSymbol{0}, State{1}, StackSymbol{2});
        transition->add(State{4}, CallSymbol{0}, State{1}, StackSymbol{1});

        transition->add(State{0}, CallSymbol{1}, State{4}, StackSymbol{1});
        transition->add(State{1}, CallSymbol{1}, State{4}, StackSymbol{2});
        transition->add(State{2}, CallSymbol{1}, State{3}, StackSymbol{1});
        transition->add(State{3}, CallSymbol{1}, State{3}, StackSymbol{1});
        transition->add(State{4}, CallSymbol{1}, State{2}, StackSymbol{2});
    }

    std::unique_ptr<common::transition::Transition<AutomatonKind::Combined>> transition;
    EmptinessChecker sut{numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols};
};

TEST_F(TestEmptinessChecker, foursStatesAccept)
{
    std::vector<uint16_t> acceptingStates{1, 2, 3, 4};
    auto vpa = std::make_shared<common::VPA<AutomatonKind::Combined>>(
        std::move(transition), initialState, acceptingStates, numOfStates);

    common::Word exptedExample{LocalSymbol{0}};

    EXPECT_EQ(*sut.check(vpa), exptedExample);
}

TEST_F(TestEmptinessChecker, oneStateAccepts)
{
    std::vector<uint16_t> acceptingStates{3};
    auto vpa = std::make_shared<common::VPA<AutomatonKind::Combined>>(
        std::move(transition), initialState, acceptingStates, numOfStates);

    common::Word exptedExample{CallSymbol{1}, LocalSymbol{2}, ReturnSymbol{0}};

    EXPECT_EQ(*sut.check(vpa), exptedExample);
}

TEST_F(TestEmptinessChecker, emptyLanguage)
{
    std::vector<uint16_t> acceptingStates{};
    auto vpa = std::make_shared<common::VPA<AutomatonKind::Combined>>(
        std::move(transition), initialState, acceptingStates, numOfStates);

    common::Word exptedExample{};

    EXPECT_EQ(*sut.check(vpa), exptedExample);
}

} // namespace teacher