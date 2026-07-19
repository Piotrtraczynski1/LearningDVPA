#include "generator/srs/XmlGenerator.hpp"

#include "utils/Constants.hpp"
#include "utils/ExitCode.hpp"
#include "utils/log.hpp"

namespace generator::srs
{
namespace
{
using State = common::transition::State;
using CallSymbol = common::symbol::CallSymbol;
using LocalSymbol = common::symbol::LocalSymbol;
using ReturnSymbol = common::symbol::ReturnSymbol;
using StackSymbol = common::symbol::StackSymbol;

enum XmlState : uint16_t
{
    Initial,
    RootTagOpened,
    RootAttributeName,
    RootAfterEquals,
    RootAttributeValue,
    RootAfterAttribute,
    RootBody,
    RootText,
    NestedTagOpened,
    NestedAttributeName,
    NestedAfterEquals,
    NestedAttributeValue,
    NestedAfterAttribute,
    NestedBody,
    NestedText,
    ChildCompleted,
    RootCompleted
};

enum XmlLocal : uint16_t
{
    Letter,
    Space,
    Equals,
    Quote,
    TagBodyStart
};
} // namespace

void XmlGenerator::validateGeneratorConfig(
    uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_, uint16_t &numOfReturns_,
    uint16_t &numOfStackSymbols_)
{
    const uint16_t adjustedNumOfStackSymbols{static_cast<uint16_t>(2 * numOfCalls + 1)};
    if (numOfStates != numOfXmlStates)
    {
        WRN("[XmlGenerator] adjusting numOfStates to %u", numOfXmlStates);
    }
    if (numOfLocals != numOfXmlLocals)
    {
        WRN("[XmlGenerator] adjusting numOfLocals to %u", numOfXmlLocals);
    }
    if (numOfReturns != numOfCalls)
    {
        WRN("[XmlGenerator] adjusting numOfReturns to %u", numOfCalls);
    }
    if (numOfStackSymbols != adjustedNumOfStackSymbols)
    {
        WRN("[XmlGenerator] adjusting numOfStackSymbols to %u", adjustedNumOfStackSymbols);
    }

    numOfStates = numOfStates_ = numOfXmlStates;
    numOfLocals = numOfLocals_ = numOfXmlLocals;
    numOfReturns = numOfReturns_ = numOfCalls;
    numOfStackSymbols = numOfStackSymbols_ = adjustedNumOfStackSymbols;
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> XmlGenerator::run()
{
    transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

    for (uint16_t tag = 0; tag < numOfCalls; tag++)
    {
        const StackSymbol rootTag{static_cast<uint16_t>(tag + 1)};
        const StackSymbol nestedTag{static_cast<uint16_t>(tag + numOfCalls + 1)};

        transition->add(State{Initial}, CallSymbol{tag}, State{RootTagOpened}, rootTag);
        transition->add(State{RootBody}, CallSymbol{tag}, State{NestedTagOpened}, nestedTag);
        transition->add(State{NestedBody}, CallSymbol{tag}, State{NestedTagOpened}, nestedTag);

        transition->add(State{RootTagOpened}, rootTag, ReturnSymbol{tag}, State{RootCompleted});
        transition->add(
            State{NestedTagOpened}, nestedTag, ReturnSymbol{tag}, State{ChildCompleted});
        transition->add(State{RootText}, rootTag, ReturnSymbol{tag}, State{RootCompleted});
        transition->add(State{NestedText}, nestedTag, ReturnSymbol{tag}, State{ChildCompleted});
        transition->add(State{ChildCompleted}, rootTag, ReturnSymbol{tag}, State{RootCompleted});
        transition->add(State{ChildCompleted}, nestedTag, ReturnSymbol{tag}, State{ChildCompleted});
    }

    transition->add(State{RootTagOpened}, LocalSymbol{TagBodyStart}, State{RootBody});
    transition->add(State{RootTagOpened}, LocalSymbol{Space}, State{RootAttributeName});
    transition->add(State{NestedTagOpened}, LocalSymbol{TagBodyStart}, State{NestedBody});
    transition->add(State{NestedTagOpened}, LocalSymbol{Space}, State{NestedAttributeName});

    transition->add(State{RootAttributeName}, LocalSymbol{Letter}, State{RootAttributeName});
    transition->add(State{RootAttributeName}, LocalSymbol{Equals}, State{RootAfterEquals});
    transition->add(State{RootAfterEquals}, LocalSymbol{Quote}, State{RootAttributeValue});
    transition->add(State{RootAttributeValue}, LocalSymbol{Letter}, State{RootAttributeValue});
    transition->add(State{RootAttributeValue}, LocalSymbol{Quote}, State{RootAfterAttribute});
    transition->add(State{RootAfterAttribute}, LocalSymbol{Space}, State{RootAttributeName});
    transition->add(State{RootAfterAttribute}, LocalSymbol{TagBodyStart}, State{RootBody});

    transition->add(State{NestedAttributeName}, LocalSymbol{Letter}, State{NestedAttributeName});
    transition->add(State{NestedAttributeName}, LocalSymbol{Equals}, State{NestedAfterEquals});
    transition->add(State{NestedAfterEquals}, LocalSymbol{Quote}, State{NestedAttributeValue});
    transition->add(State{NestedAttributeValue}, LocalSymbol{Letter}, State{NestedAttributeValue});
    transition->add(State{NestedAttributeValue}, LocalSymbol{Quote}, State{NestedAfterAttribute});
    transition->add(State{NestedAfterAttribute}, LocalSymbol{Space}, State{NestedAttributeName});
    transition->add(State{NestedAfterAttribute}, LocalSymbol{TagBodyStart}, State{NestedBody});

    transition->add(State{RootBody}, LocalSymbol{Letter}, State{RootText});
    transition->add(State{RootText}, LocalSymbol{Letter}, State{RootText});
    transition->add(State{NestedBody}, LocalSymbol{Letter}, State{NestedText});
    transition->add(State{NestedText}, LocalSymbol{Letter}, State{NestedText});

    return std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), State{Initial}, std::vector<uint16_t>{RootCompleted}, numOfStates);
}

learner::srs::Srs XmlGenerator::getSrs()
{
    learner::srs::Srs srs{};
    const auto addRule = [&srs](const common::Word &left, const common::Word &right)
    {
        srs.push_back(
            learner::srs::SrsRuleWithParams{
                .left = {.left = left, .takesParams = false},
                .right = {.left = right, .takesParams = false}});
    };

    // <a> --> <a att = "text"> (saves 8 queries)
    addRule(
        common::Word{CallSymbol{0}, LocalSymbol{TagBodyStart}},
        common::Word{
            CallSymbol{0}, LocalSymbol{Space}, LocalSymbol{Letter}, LocalSymbol{Equals},
            LocalSymbol{Quote}, LocalSymbol{Letter}, LocalSymbol{Quote},
            LocalSymbol{TagBodyStart}});

    // <a/> --> <a> text </a> (saves 2 queries)
    addRule(
        common::Word{CallSymbol{0}, ReturnSymbol{0}},
        common::Word{
            CallSymbol{0}, LocalSymbol{TagBodyStart}, LocalSymbol{Letter}, ReturnSymbol{0}});

    // <a> text </a> --> <a> <a/> </a> (saves 1 query)
    addRule(
        common::Word{
            CallSymbol{0}, LocalSymbol{TagBodyStart}, LocalSymbol{Letter}, ReturnSymbol{0}},
        common::Word{
            CallSymbol{0}, LocalSymbol{TagBodyStart}, CallSymbol{0}, ReturnSymbol{0},
            ReturnSymbol{0}});

    return srs;
}
} // namespace generator::srs
