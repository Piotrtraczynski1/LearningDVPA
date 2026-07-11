#include <limits>

#include "utils/Counters.hpp"
#include "utils/log.hpp"

std::unordered_map<std::string, uint64_t> Counters::counters;

void Counters::update(std::string_view counter, uint64_t val)
{
    auto [it, inserted] = counters.try_emplace(std::string(counter), val);
    if (not inserted)
    {
        if (val > std::numeric_limits<uint64_t>::max() - it->second)
        {
            ERR("Counter overflow!");
            exit(1);
        }
        it->second += val;
    }
}

void Counters::reset()
{
    counters.clear();
}

uint64_t Counters::getCounter(const std::string_view counter)
{
    return counters[std::string(counter)];
}