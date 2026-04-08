#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "learner/srs/SpecialSymbolAdder.hpp"

using namespace common::symbol;
using common::transition::State;

namespace learner::srs
{
class TestSpecialSymbolAdder : public ::testing::Test
{
public:
    void SetUp() override
    {
        initTransition();
        addPrefixes();

        hypothesis = std::make_shared<common::VPA<AutomatonKind::Normal>>(
            std::move(transition), initialState, acceptingStates, numOfStates);
        sut = std::make_shared<SpecialSymbolAdder>(hypothesis);
    }

    void initTransition()
    {
        transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();
        // Locals:
        transition->add(State{0}, LocalSymbol{0}, State{1});
        transition->add(State{1}, LocalSymbol{0}, State{2});
        transition->add(State{2}, LocalSymbol{0}, State{3});
        transition->add(State{3}, LocalSymbol{0}, State{4});
        transition->add(State{4}, LocalSymbol{0}, State{0});

        transition->add(State{0}, LocalSymbol{1}, State{4});
        transition->add(State{1}, LocalSymbol{1}, State{3});
        transition->add(State{2}, LocalSymbol{1}, State{2});
        transition->add(State{3}, LocalSymbol{1}, State{4});
        transition->add(State{4}, LocalSymbol{1}, State{1});

        // Calls:
        transition->add(State{0}, CallSymbol{0}, State{4}, StackSymbol{1});
        transition->add(State{1}, CallSymbol{0}, State{3}, StackSymbol{1});
        transition->add(State{2}, CallSymbol{0}, State{3}, StackSymbol{1});
        transition->add(State{3}, CallSymbol{0}, State{2}, StackSymbol{1});
        transition->add(State{4}, CallSymbol{0}, State{0}, StackSymbol{1});

        // Returns:
        transition->add(State{0}, StackSymbol{0}, ReturnSymbol{0}, State{0});
        transition->add(State{1}, StackSymbol{0}, ReturnSymbol{0}, State{1});
        transition->add(State{2}, StackSymbol{0}, ReturnSymbol{0}, State{3});
        transition->add(State{3}, StackSymbol{0}, ReturnSymbol{0}, State{2});
        transition->add(State{4}, StackSymbol{0}, ReturnSymbol{0}, State{0});

        transition->add(State{0}, StackSymbol{1}, ReturnSymbol{0}, State{4});
        transition->add(State{1}, StackSymbol{1}, ReturnSymbol{0}, State{1});
        transition->add(State{2}, StackSymbol{1}, ReturnSymbol{0}, State{1});
        transition->add(State{3}, StackSymbol{1}, ReturnSymbol{0}, State{3});
        transition->add(State{4}, StackSymbol{1}, ReturnSymbol{0}, State{4});
    }

    void addPrefixes()
    {
        prefixes.push_back(common::Word{});
        prefixes.push_back(common::Word{LocalSymbol{0}});
        prefixes.push_back(common::Word{LocalSymbol{0}, LocalSymbol{0}});
        prefixes.push_back(common::Word{LocalSymbol{0}, LocalSymbol{0}, LocalSymbol{0}});
        prefixes.push_back(
            common::Word{LocalSymbol{0}, LocalSymbol{0}, LocalSymbol{0}, LocalSymbol{0}});
    }

    void testResult(const ConvertedAutomata &convertedAutomata, SrsRule rule)
    {
        for (const auto &prefix : prefixes)
        {
            ASSERT_EQ(
                hypothesis->checkWord(prefix + rule.left),
                convertedAutomata.lAutomaton->checkWord(prefix + common::Word{specialSymbol}))
                << "prefix: " << prefix << ", l = " << rule.left;
            ASSERT_EQ(
                hypothesis->checkWord(prefix + rule.right),
                convertedAutomata.rAutomaton->checkWord(prefix + common::Word{specialSymbol}))
                << "prefix: " << prefix << ", r = " << rule.right;
        }
    }

    std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> transition;
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis;

    const State initialState{0};
    const LocalSymbol specialSymbol{2};
    const uint16_t numOfStates{5};
    const std::vector<uint16_t> acceptingStates{0, 2, 4};

    std::vector<common::Word> prefixes{};

    std::shared_ptr<SpecialSymbolAdder> sut;
};

TEST_F(TestSpecialSymbolAdder, defaultTest1)
{
    SrsRule rule{
        .left = common::Word{LocalSymbol{0}, LocalSymbol{1}},
        .right = common::Word{CallSymbol{0}, ReturnSymbol{0}}};

    sut->addNewRule(specialSymbol, rule);
    auto convertedAutomata{sut->getConvertedAutomata()};
    testResult(*convertedAutomata, rule);
}

TEST_F(TestSpecialSymbolAdder, defaultTest2)
{
    SrsRule rule{
        .left = common::Word{CallSymbol{0}, LocalSymbol{0}, LocalSymbol{1}, ReturnSymbol{0}},
        .right = common::Word{
            CallSymbol{0}, ReturnSymbol{0}, CallSymbol{0}, ReturnSymbol{0}, LocalSymbol{1},
            CallSymbol{0}, ReturnSymbol{0}}};

    sut->addNewRule(specialSymbol, rule);
    auto convertedAutomata{sut->getConvertedAutomata()};
    testResult(*convertedAutomata, rule);
}

} // namespace learner::srs
