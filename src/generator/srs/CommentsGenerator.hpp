#pragma once

#include <cstdint>
#include <memory>

#include "common/VPA.hpp"
#include "generator/RandomGenerator.hpp"
#include "learner/srs/Srs.hpp"

namespace generator::srs
{
class CommentsGenerator : public RandomGenerator
{
public:
    using RandomGenerator::RandomGenerator;

    std::shared_ptr<common::VPA<AutomatonKind::Normal>> run() override;
    learner::srs::Srs getSrs() override;
    bool generatorSpecificCheck(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis) override;

private:
    void validateGeneratorConfig(
        uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_,
        uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_) override;

    void addComments();
};
} // namespace generator::srs