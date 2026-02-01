#include "learner/Calculator.hpp"

namespace learner
{
Calculator::Calculator(
    const teacher::Teacher &teacher, std::shared_ptr<Selectors> s, std::shared_ptr<TestWords> tw)
    : oracle(teacher), selectors(s), testWords(tw) {};

bool Calculator::isRightCongruence(const common::Word &a, const common::Word &b) const
{
    for (int i = 0; i < testWords->size(); i++)
    {
        if (areDistinguishable(a + (*testWords)[i], b + (*testWords)[i]))
        {
            return false;
        }
    }
    return true;
}

bool Calculator::areDistinguishable(const common::Word &a, const common::Word &b) const
{
    if (oracle.membershipQuery(a) != oracle.membershipQuery(b))
    {
        return true;
    }
    if (oracle.stackContentQuery(a) != oracle.stackContentQuery(b))
    {
        return true;
    }
    return false;
}

uint16_t Calculator::findEquivalentSelector(const common::Word &word)
{
    for (uint16_t selector = 0; selector < selectors->size(); selector++)
    {
        if (isRightCongruence((*selectors)[selector], word))
        {
            return selector;
        }
    }
    return Selectors::INVALID_INDEX;
}

uint16_t Calculator::addSelector(const common::Word &selector)
{
    bool isAccepting{oracle.membershipQuery(selector)};
    selectors->addSelector(selector, isAccepting, oracle.stackContentQuery(selector).top());
    return selectors->size() - 1;
}

uint16_t Calculator::findOrAddSuccessor(const common::Word &candidate)
{
    uint16_t successor{findEquivalentSelector(candidate)};

    if (successor == Selectors::INVALID_INDEX)
    {
        successor = addSelector(candidate);
    }

    return successor;
}
} // namespace learner
