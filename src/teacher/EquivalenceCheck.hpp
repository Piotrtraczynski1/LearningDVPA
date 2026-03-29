#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "teacher/Converter.hpp"

namespace teacher
{
std::shared_ptr<common::Word> equivalenceCheck(
    std::shared_ptr<teacher::Converter> converter,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa);
} // namespace teacher
