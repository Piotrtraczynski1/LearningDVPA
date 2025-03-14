#pragma once

#include <chrono>
#include <iostream>
#include <map>

struct MarkerInfo
{
    uint64_t time{0};
    uint64_t executions{0};
};

class MeasurementDataBase
{
    static std::map<std::string, MarkerInfo> markers;

public:
    static void update(const std::string &name, uint64_t duration);

    static void printInfo();
};

class Measurment
{
    std::chrono::time_point<std::chrono::high_resolution_clock> starTime;
    std::string name;

public:
    Measurment(std::string n);
    ~Measurment();
};

#define TIME_MARKER(NAME) Measurment _measurement(NAME);
