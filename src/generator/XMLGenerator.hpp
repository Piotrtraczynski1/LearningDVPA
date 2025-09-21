#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"
#include "generator/Generator.hpp"

namespace generator
{
class XMLGenerator : public Generator
{
public:
    using Generator::Generator;

    std::shared_ptr<common::VPA> run() override;
    bool generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis) override;

private:
    std::vector<uint16_t> acceptingStates{0};
    common::transition::State initialState{0};

    void generateTransition();
};
} // namespace generator
