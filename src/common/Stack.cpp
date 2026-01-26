#include "common/Stack.hpp"

namespace common
{
Stack::Stack(std::initializer_list<symbol::StackSymbol> initList)
{
    for (auto it = initList.begin(); it != initList.end(); ++it)
    {
        stack.push(*it);
    }
}

symbol::StackSymbol Stack::top() const
{
    if (stack.empty())
    {
        return symbol::StackSymbol::BOTTOM;
    }
    return stack.top();
}

void Stack::pop()
{
    if (not stack.empty())
    {
        stack.pop();
    }
}

void Stack::push(symbol::StackSymbol symbol)
{
    stack.push(symbol);
}

size_t Stack::size() const
{
    return stack.size();
}

std::ostream &operator<<(std::ostream &os, const Stack &stack)
{
    Stack copy{stack};
    os << "{";
    while (copy.size() > 0)
    {
        os << copy.top() << ", ";
        copy.pop();
    }
    os << "}";
    return os;
}

} // namespace common
