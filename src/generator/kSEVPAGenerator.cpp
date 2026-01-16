#include "generator/kSEVPAGenerator.hpp"

namespace generator
{
bool kSEVPAGenerator::generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis)
{
}

std::shared_ptr<common::VPA> kSEVPAGenerator::run()
{
    transition->clear();
    
}
} // namespace generator