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
    Teacher(std::shared_ptr<common::VPA<AutomatonKind::Normal>> automaton, std::shared_ptr<teacher::Converter> conv)
        : vpa{automaton}, converter{conv} {};

    bool membershipQuery(const common::Word &word) const;
    common::Stack stackContentQuery(const common::Word &word) const;
    std::shared_ptr<common::Word>
    equivalenceQuery(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis) const;

private:
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa;
    std::shared_ptr<teacher::Converter> converter;
};
} // namespace teacher