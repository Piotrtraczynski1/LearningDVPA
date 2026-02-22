#pragma once

#include "generator/CdaGenerator.hpp"
#include "utils/Constants.hpp"

namespace generator
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
} // namespace generator
