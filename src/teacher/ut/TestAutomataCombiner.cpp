#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"
#include "teacher/AutomataCombiner.hpp"

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
class TestAutomataCombiner : public Test
{
public:
    void SetUp() override
    {
        initTransition();

        firstVpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
            std::move(transition), initialState, acceptingStates, numOfStates);
        sut = std::make_shared<AutomataCombiner<AutomatonKind::Combined>>(
            firstVpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

        initTestWords();
    }

    void initTransition()
    {
        transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

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

    void initTestWords()
    {
        testWords.push_back(common::Word{});
        testWords.push_back(
            common::Word{CallSymbol{0}, CallSymbol{1}, ReturnSymbol{0}, LocalSymbol{0}});
        testWords.push_back(common::Word{LocalSymbol{0}, LocalSymbol{1}, LocalSymbol{2}});
        testWords.push_back(
            common::Word{
                CallSymbol{1}, ReturnSymbol{0}, ReturnSymbol{0}, ReturnSymbol{0}, LocalSymbol{2}});
        testWords.push_back(
            common::Word{
                LocalSymbol{1}, CallSymbol{1}, LocalSymbol{2}, ReturnSymbol{0}, ReturnSymbol{0},
                ReturnSymbol{0}, LocalSymbol{2}});
        testWords.push_back(
            common::Word{
                LocalSymbol{2}, ReturnSymbol{0}, CallSymbol{0}, ReturnSymbol{0}, ReturnSymbol{0},
                LocalSymbol{1}});
        testWords.push_back(common::Word{LocalSymbol{2}, CallSymbol{2}});
    }

    void checkResult(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> secondVpa,
        std::shared_ptr<common::VPA<AutomatonKind::Combined>> combinedVpa)
    {
        for (const auto &word : testWords)
        {
            ASSERT_EQ(
                firstVpa->checkWord(word) ^ secondVpa->checkWord(word),
                combinedVpa->checkWord(word))
                << word << ", firstVpa: " << firstVpa->checkWord(word)
                << ", secondVpa: " << secondVpa->checkWord(word)
                << ", combinedVpa: " << combinedVpa->checkWord(word);
        }
    }

    std::vector<common::Word> testWords{};
    std::vector<uint16_t> acceptingStates = {1, 3, 4};
    std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> transition;
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> firstVpa;
    std::shared_ptr<AutomataCombiner<AutomatonKind::Combined>> sut;
};

TEST_F(TestAutomataCombiner, default1)
{
    constexpr uint16_t secondVpaNumOfStates{3};
    auto secondTransition =
        std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

    // locals
    secondTransition->add(State{0}, LocalSymbol{0}, State{1});
    secondTransition->add(State{1}, LocalSymbol{0}, State{2});
    secondTransition->add(State{2}, LocalSymbol{0}, State{0});

    secondTransition->add(State{0}, LocalSymbol{1}, State{2});
    secondTransition->add(State{1}, LocalSymbol{1}, State{1});
    secondTransition->add(State{2}, LocalSymbol{1}, State{2});

    secondTransition->add(State{0}, LocalSymbol{2}, State{0});
    secondTransition->add(State{1}, LocalSymbol{2}, State{0});
    secondTransition->add(State{2}, LocalSymbol{2}, State{1});

    // returns
    secondTransition->add(State{0}, StackSymbol{0}, ReturnSymbol{0}, State{2});
    secondTransition->add(State{1}, StackSymbol{0}, ReturnSymbol{0}, State{2});
    secondTransition->add(State{2}, StackSymbol{0}, ReturnSymbol{0}, State{1});

    secondTransition->add(State{0}, StackSymbol{1}, ReturnSymbol{0}, State{1});
    secondTransition->add(State{1}, StackSymbol{1}, ReturnSymbol{0}, State{2});
    secondTransition->add(State{2}, StackSymbol{1}, ReturnSymbol{0}, State{1});

    secondTransition->add(State{0}, StackSymbol{2}, ReturnSymbol{0}, State{0});
    secondTransition->add(State{1}, StackSymbol{2}, ReturnSymbol{0}, State{2});
    secondTransition->add(State{2}, StackSymbol{2}, ReturnSymbol{0}, State{0});

    // calls
    secondTransition->add(State{0}, CallSymbol{0}, State{1}, StackSymbol{2});
    secondTransition->add(State{1}, CallSymbol{0}, State{1}, StackSymbol{1});
    secondTransition->add(State{2}, CallSymbol{0}, State{2}, StackSymbol{2});

    secondTransition->add(State{0}, CallSymbol{1}, State{2}, StackSymbol{1});
    secondTransition->add(State{1}, CallSymbol{1}, State{2}, StackSymbol{2});
    secondTransition->add(State{2}, CallSymbol{1}, State{0}, StackSymbol{1});

    auto secondVpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(secondTransition), initialState, std::vector<uint16_t>{0, 2},
        secondVpaNumOfStates);

    checkResult(secondVpa, sut->combineVPA(*secondVpa));
}

TEST_F(TestAutomataCombiner, default2)
{
    constexpr uint16_t secondVpaNumOfStates{2};
    auto secondTransition =
        std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

    // locals
    secondTransition->add(State{0}, LocalSymbol{0}, State{0});
    secondTransition->add(State{1}, LocalSymbol{0}, State{1});

    secondTransition->add(State{0}, LocalSymbol{1}, State{1});
    secondTransition->add(State{1}, LocalSymbol{1}, State{1});

    secondTransition->add(State{0}, LocalSymbol{2}, State{0});
    secondTransition->add(State{1}, LocalSymbol{2}, State{0});

    // returns
    secondTransition->add(State{0}, StackSymbol{0}, ReturnSymbol{0}, State{1});
    secondTransition->add(State{1}, StackSymbol{0}, ReturnSymbol{0}, State{0});

    secondTransition->add(State{0}, StackSymbol{1}, ReturnSymbol{0}, State{0});
    secondTransition->add(State{1}, StackSymbol{1}, ReturnSymbol{0}, State{1});

    secondTransition->add(State{0}, StackSymbol{2}, ReturnSymbol{0}, State{0});
    secondTransition->add(State{1}, StackSymbol{2}, ReturnSymbol{0}, State{1});

    // calls
    secondTransition->add(State{0}, CallSymbol{0}, State{1}, StackSymbol{2});
    secondTransition->add(State{1}, CallSymbol{0}, State{1}, StackSymbol{1});

    secondTransition->add(State{0}, CallSymbol{1}, State{0}, StackSymbol{1});
    secondTransition->add(State{1}, CallSymbol{1}, State{1}, StackSymbol{2});

    auto secondVpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(secondTransition), initialState, std::vector<uint16_t>{1}, secondVpaNumOfStates);

    checkResult(secondVpa, sut->combineVPA(*secondVpa));
}
} // namespace teacher