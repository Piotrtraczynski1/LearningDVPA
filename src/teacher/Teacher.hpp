#pragma once

#include <memory>

#include "../common/Stack.hpp"
#include "../common/VPA.hpp"
#include "../common/Word.hpp"

namespace teacher
{
class Teacher
{
public:
    bool membershipQuery(common::Word word) const;
    common::Stack stackContentQuery(common::Word word) const;
    std::shared_ptr<common::Word> equivalenceQuery(common::VPA vpa) const;

private:
    void printSymbol(const common::Symbol sym) const;
};
} // namespace teacher