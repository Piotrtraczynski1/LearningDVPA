#include <iostream>

#include "../common/VPA.hpp"
#include "Teacher.hpp"

namespace teacher
{
void Teacher::printSymbol(const common::Symbol sym) const
{
    switch (sym.index())
    {
    case 0:
    {
        std::cout << "CallSymbol{" << std::get<common::symbol::CallSymbol>(sym) << "}, ";
        break;
    }
    case 1:
    {
        std::cout << "ReturnSymbol{" << std::get<common::symbol::ReturnSymbol>(sym) << "}, ";
        break;
    }
    case 2:
    {
        std::cout << "LocalSymbol{" << std::get<common::symbol::LocalSymbol>(sym) << "}, ";
        break;
    }
    case 3:
    {
        common::Stack s = std::get<common::Stack>(sym);
        std::cout << "ControlWord: ";
        while (not(s.size() == 0))
        {
            std::cout << s.top() << ", ";
            s.pop();
        }
        break;
    }
    }
    std::cout << std::endl;
}

bool Teacher::membershipQuery(common::Word word) const
{
    std::cout << "membershipQuery word:\n";
    for (auto symbol : word)
    {
        printSymbol(symbol);
    }
    bool ans;
    std::cin >> ans;
    return ans;
}

common::Stack Teacher::stackContentQuery(common::Word word) const
{
    std::cout << "stackContentQuery word:\n";
    for (auto symbol : word)
    {
        printSymbol(symbol);
    }
    common::Stack ans;

    while (true)
    {
        char in;
        std::cin >> in;
        if (in == 'Q')
        {
            return ans;
        }
        ans.push(common::symbol::StackSymbol{static_cast<uint16_t>(in - '0')});
    }
}

std::shared_ptr<common::Word> Teacher::equivalenceQuery(common::VPA vpa) const
{
    static bool isFirstQuery{true};

    if (isFirstQuery)
    {
        return std::make_shared<common::Word>(
            common::Word{common::symbol::CallSymbol{1}, common::symbol::ReturnSymbol{1}});
    }
    return nullptr;
}
} // namespace teacher