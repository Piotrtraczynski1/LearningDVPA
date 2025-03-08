#pragma once

#include <memory>

#include "common/Stack.hpp"
#include "common/VPA.hpp"
#include "common/Word.hpp"

namespace teacher
{
class Teacher
{
public:
    bool membershipQuery(const common::Word &word) const;
    const common::Stack &stackContentQuery(const common::Word &word) const;
    std::shared_ptr<common::Word>
    equivalenceQuery(const std::shared_ptr<common::VPA> hypothesis) const;
    Teacher(std::shared_ptr<common::VPA> automata) : vpa(automata)
    {
    }

private:
    std::shared_ptr<common::VPA> vpa;
    mutable bool isFirstQuery{true};
};
} // namespace teacher