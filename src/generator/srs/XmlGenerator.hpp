#pragma once

#include "generator/Generator.hpp"

namespace generator::srs
{
class XmlGenerator : public Generator
{
public:
    using Generator::Generator;

    std::shared_ptr<common::VPA<AutomatonKind::Normal>> run() override;
    learner::srs::Srs getSrs() override;

private:
    void validateGeneratorConfig(
        uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_,
        uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_) override;

    static constexpr uint16_t numOfXmlStates{17};
    static constexpr uint16_t numOfXmlLocals{5};
};
} // namespace generator::srs
