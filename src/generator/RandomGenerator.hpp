#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"
#include "generator/Generator.hpp"

namespace generator
{
class RandomGenerator : public Generator
{
public:
    using Generator::Generator;

    std::shared_ptr<common::VPA> run() override;
    bool generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis) override;

private:
    void generateTransition();
    std::vector<uint16_t> selectAcceptingStates();

    void addCalls(common::transition::State state);
    void addLocals(common::transition::State state);
    void addReturns(common::transition::State state);
    bool skipTransition();
};
} // namespace generator
