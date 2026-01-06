#pragma once

#include <chrono>
#include <iostream>
#include <unordered_map>

struct MarkerInfo
{
    uint64_t time{0};
    uint64_t executions{0};
};

class MeasurementDataBase
{
    static std::unordered_map<std::string, MarkerInfo> markers;

public:
    static void update(const std::string_view name, uint64_t duration);

    static void printInfo();

    static void reset();
    static MarkerInfo getMarkerInfo(const std::string_view name);
};

class Measurment
{
    std::chrono::steady_clock::time_point starTime;
    std::string_view name;

public:
    Measurment(const std::string_view n);
    ~Measurment();
};

#define TIME_MARKER(NAME) Measurment _measurement(NAME);
