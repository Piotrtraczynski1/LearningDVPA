#pragma once

#include <memory>

#include "common/Stack.hpp"
#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "teacher/Converter.hpp"
#include "utils/log.hpp"

namespace teacher
{
class Teacher
{
public:
    Teacher(std::shared_ptr<common::VPA> automata, std::shared_ptr<teacher::Converter> conv)
        : vpa{automata}, converter{conv}
    {
        IMP("[Teacher]: built");
    };

    bool membershipQuery(const common::Word &word) const;
    const common::Stack &stackContentQuery(const common::Word &word) const;
    std::shared_ptr<common::Word>
    equivalenceQuery(const std::shared_ptr<common::VPA> hypothesis) const;

private:
    std::shared_ptr<common::VPA> vpa;
    std::shared_ptr<teacher::Converter> converter;
};
} // namespace teacher