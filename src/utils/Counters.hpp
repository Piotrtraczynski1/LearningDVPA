#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

class Counters
{
    static std::unordered_map<std::string, uint64_t> counters;

public:
    static void update(std::string_view counter, uint64_t val);
    static void reset();
    static uint64_t getCounter(const std::string_view counter);
};