#pragma once

#include "generator/callDriven/CdaGenerator.hpp"
#include "utils/Constants.hpp"

namespace generator::callDriven
{
class SeVpaGenerator : public CdaGenerator
{
public:
    using CdaGenerator::CdaGenerator;

    void selectTargetsForCalls() override;

private:
    void selectEntryPoints();

    uint16_t entryPoints[utils::MaxNumOfCombinedAutomatonLetters];
};
} // namespace generator::callDriven
