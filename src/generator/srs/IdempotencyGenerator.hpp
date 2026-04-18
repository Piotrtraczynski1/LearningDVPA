#pragma once

#include "generator/callDriven/SeVpaGenerator.hpp"
#include "learner/srs/Srs.hpp"

namespace generator::srs
{
class IdempotencyGenerator : public callDriven::SeVpaGenerator
{
public:
    using SeVpaGenerator::SeVpaGenerator;
    learner::srs::Srs getSrs() override;

private:
    void addReturns() override;
    void addCalls() override;
};
} // namespace generator::srs
