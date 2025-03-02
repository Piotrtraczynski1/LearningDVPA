#include "Word.hpp"

namespace common
{
Word operator+(const Word &a, const Word &b)
{
    Word res = a;
    res.insert(res.end(), b.begin(), b.end());
    return res;
}
} // namespace common