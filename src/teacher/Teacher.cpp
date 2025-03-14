#include <iostream>

#include "common/VPA.hpp"
#include "teacher/Teacher.hpp"
#include "teacher/cfg/Calculator.hpp"
#include "utils/TimeMarker.hpp"

namespace teacher
{
bool Teacher::membershipQuery(const common::Word &word) const
{
    TIME_MARKER("membershipQuery");

    return vpa->checkWord(word);
}

const common::Stack &Teacher::stackContentQuery(const common::Word &word) const
{
    TIME_MARKER("stackContentQuery");

    vpa->checkWord(word);
    return vpa->stack;
}

std::shared_ptr<common::Word>
Teacher::equivalenceQuery(const std::shared_ptr<common::VPA> hypothesis) const
{
    TIME_MARKER("equivalenceQuery");

    common::VPA combinedVpa{converter->combineVPA(*hypothesis)};
    std::shared_ptr<cfg::Cfg> cfg{converter->convertVpaToCfg(combinedVpa)};
    auto cfgOutput{cfg->isEmpty()};

    return cfg::Calculator::convertCfgOutputToWord(*cfgOutput);
}
} // namespace teacher