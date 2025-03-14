#include <variant>

#include "common/Word.hpp"

namespace common
{
Word operator+(const Word &a, const Word &b)
{
    Word res = a;
    res.insert(res.end(), b.begin(), b.end());
    return res;
}

Word &operator+=(Word &lhs, const Word &rhs)
{
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}

Word &operator+=(Word &lhs, const Symbol &rhs)
{
    lhs.push_back(rhs);
    return lhs;
}

std::ostream &operator<<(std::ostream &os, const Symbol &symbol)
{
    switch (symbol.index())
    {
    case 0:
    {
        os << "C-" << std::get<symbol::CallSymbol>(symbol);
        break;
    }
    case 1:
    {
        os << "R-" << std::get<symbol::ReturnSymbol>(symbol);
        break;
    }
    case 2:
    {
        os << "L-" << std::get<symbol::LocalSymbol>(symbol);
        break;
    }
    case 3:
    {
        os << "CW-" << std::get<Stack>(symbol);
        break;
    }
    }
    return os;
}
std::ostream &operator<<(std::ostream &os, const Word &word)
{
    os << "[";
    for (uint16_t i = 0; i < word.size(); i++)
    {
        os << word[i];
        if (i + 1 < word.size())
            os << ", ";
    }
    return os << "]";
}
} // namespace common