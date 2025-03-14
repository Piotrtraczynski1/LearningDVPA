#pragma once

#include <iostream>
#include <stack>

#include "common/symbol/Symbols.hpp"

namespace common
{
class Stack
{
    std::stack<symbol::StackSymbol> stack;

public:
    Stack() = default;
    Stack(std::initializer_list<symbol::StackSymbol> initList);
    bool operator!=(const Stack &s) const
    {
        return stack != s.stack;
    };

    // dummy operator
    bool operator==(const Stack &s1) const
    {
        return true;
    }

    symbol::StackSymbol top() const;
    void pop();
    void push(symbol::StackSymbol symbol);
    size_t size() const;
};

std::ostream &operator<<(std::ostream &os, const Stack &stack);
} // namespace common