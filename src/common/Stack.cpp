#include "Stack.hpp"

namespace common
{
symbol::StackSymbol Stack::top()
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

void Stack::push(symbol::StackSymbol symbol) { stack.push(symbol); }
} // namespace common