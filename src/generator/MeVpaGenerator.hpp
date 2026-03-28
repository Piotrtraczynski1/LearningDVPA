#pragma once

#include "generator/CdaGenerator.hpp"
#include "utils/Constants.hpp"

namespace generator
{
class MeVpaGenerator : public CdaGenerator
{
public:
    using CdaGenerator::CdaGenerator;

    void validateGeneratorConfig() override;

    common::symbol::StackSymbol selectStackSymbol(
        const uint16_t module, [[maybe_unused]] const uint16_t callId) const override;
    common::transition::State findSuccessorForReturn(const uint16_t stackSymbol) const override;
};
} // namespace generator
