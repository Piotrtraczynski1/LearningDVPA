#include <iostream>

#include "common/VPA.hpp"
#include "teacher/Teacher.hpp"
#include "utils/log.hpp"

namespace teacher
{
bool Teacher::membershipQuery(const common::Word &word) const
{
    bool ans = vpa->checkWord(word);
    return ans;
}

const common::Stack &Teacher::stackContentQuery(const common::Word &word) const
{
    vpa->checkWord(word);
    return vpa->stack;
}

std::shared_ptr<common::Word>
Teacher::equivalenceQuery(const std::shared_ptr<common::VPA> hypothesis) const
{
    LOG("[Teacher]: equivalenceQuery\n");
    // input2.json
    common::Word w1{common::symbol::CallSymbol{0}, common::symbol::ReturnSymbol{0}};
    common::Word w2{common::symbol::LocalSymbol{0}};
    common::Word w3{
        common::symbol::LocalSymbol{0}, common::symbol::CallSymbol{0},
        common::symbol::ReturnSymbol{0}};

    if (vpa->checkWord(w1) != hypothesis->checkWord(w1))
    {
        return std::make_shared<common::Word>(w1);
    }
    if (vpa->checkWord(w2) != hypothesis->checkWord(w2))
    {
        return std::make_shared<common::Word>(w2);
    }
    if (vpa->checkWord(w3) != hypothesis->checkWord(w3))
    {
        return std::make_shared<common::Word>(w3);
    }

    /* // input onlyLocals.json
using Ls = common::symbol::LocalSymbol;
common::Word w1{Ls{0}};
common::Word w2{Ls{0}, Ls{0}};
common::Word w3{Ls{0}, Ls{0}, Ls{0}};
common::Word w4{Ls{0}, Ls{0}, Ls{0}, Ls{0}};
common::Word w5{Ls{0}, Ls{0}, Ls{0}, Ls{0}, Ls{0}};
common::Word w6{Ls{0}, Ls{0}, Ls{0}, Ls{0}, Ls{0}, Ls{0}};
if (vpa->checkWord(w1) != hypothesis.checkWord(w1))
{
return std::make_shared<common::Word>(w1);
}
if (vpa->checkWord(w2) != hypothesis.checkWord(w2))
{
LOG("[TEACHER]: testWord2: vpa=%d, hypothesis=%d", vpa->checkWord(w2),
hypothesis.checkWord(w2));
return std::make_shared<common::Word>(w2);
}
if (vpa->checkWord(w3) != hypothesis.checkWord(w3))
{
return std::make_shared<common::Word>(w3);
}
if (vpa->checkWord(w4) != hypothesis.checkWord(w4))
{
return std::make_shared<common::Word>(w4);
}
if (vpa->checkWord(w5) != hypothesis.checkWord(w5))
{
return std::make_shared<common::Word>(w5);
}
if (vpa->checkWord(w6) != hypothesis.checkWord(w6))
{
return std::make_shared<common::Word>(w6);
}*/
    return std::make_shared<common::Word>(common::Word{});
}
} // namespace teacher