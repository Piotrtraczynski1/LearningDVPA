#pragma once

#include <stack>

#include "symbol/Symbols.hpp"

namespace common
{
class Stack
{
    std::stack<symbol::StackSymbol> stack;

public:
    Stack() = default;
    Stack(std::stack<symbol::StackSymbol> &s) : stack(s) {} // TODO: do not copy the stack
    Stack(std::initializer_list<symbol::StackSymbol> initList)
    {
        for (auto it = initList.begin(); it != initList.end(); ++it)
        {
            stack.push(*it);
        }
    }

    symbol::StackSymbol top();
    void pop();
    void push(symbol::StackSymbol symbol);
};
} // namespace common