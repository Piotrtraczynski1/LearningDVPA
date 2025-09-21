#include <iostream>

#include "common/VPA.hpp"
#include "teacher/Teacher.hpp"
#include "teacher/cfg/Calculator.hpp"
#include "utils/TimeMarker.hpp"

namespace teacher
{
bool Teacher::membershipQuery(const common::Word &word) const
{
    TIME_MARKER("[Teacher]: membershipQuery");

    return vpa->checkWord(word);
}

common::Stack Teacher::stackContentQuery(const common::Word &word) const
{
    TIME_MARKER("[Teacher]: stackContentQuery");

    vpa->checkWord(word);
    return vpa->stack;
}

std::shared_ptr<common::Word>
Teacher::equivalenceQuery(const std::shared_ptr<common::VPA> hypothesis) const
{
    TIME_MARKER("[Teacher]: equivalenceQuery");

    common::VPA combinedVpa{converter->combineVPA(*hypothesis)};
    std::shared_ptr<cfg::Cfg> cfg{converter->convertVpaToCfg(combinedVpa)};
    auto cfgOutput{cfg->isEmpty()};

    std::shared_ptr<common::Word> output{cfg::Calculator::convertCfgOutputToWord(*cfgOutput)};

    if (output->size() == 0)
    {
        return output;
    }

    for (uint16_t i = 1; i <= output->size(); i++)
    {
        common::Word testWord{(*output).begin(), (*output).begin() + i};
        if (vpa->checkWord(testWord) != hypothesis->checkWord(testWord))
        {
            return std::make_shared<common::Word>(testWord);
        }
    }

    ERR("CFG output is incorrect!");
    std::cout << *output << ", vpa: " << vpa->checkWord(*output)
              << ", hypothesis: " << hypothesis->checkWord(*output)
              << ", combined: " << combinedVpa.checkWord(*output) << std::endl;
    exit(1);
}
} // namespace teacher