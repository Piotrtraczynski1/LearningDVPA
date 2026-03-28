#include <iomanip>
#include <iostream>

#include "utils/TimeMarker.hpp"

std::unordered_map<std::string, MarkerInfo> MeasurementDataBase::markers;

void MeasurementDataBase::update(const std::string_view name, uint64_t duration)
{
    auto [it, inserted] = markers.try_emplace(std::string(name), MarkerInfo{duration, 1});
    if (not inserted)
    {
        it->second.time += duration;
        it->second.executions++;
    }
}

void MeasurementDataBase::printInfo()
{
    std::cout << "=============== Time Markers stats: ===============\n";
    for (auto &marker : markers)
    {
        auto micros = marker.second.time;
        auto minutes = micros / (1000 * 1000 * 60);
        micros %= (1000 * 1000 * 60);
        auto seconds = micros / (1000 * 1000);
        micros %= (1000 * 1000);
        auto millis = micros / 1000;

        std::cout << "Marker " << std::left << std::setw(40) << marker.first << " | " << std::right
                  << std::setw(4) << minutes << "m:" << std::setw(3) << seconds
                  << "s:" << std::setw(4) << millis << "ms" << " | " << std::setw(12)
                  << marker.second.time << " us" << " | total executions: " << std::setw(10)
                  << marker.second.executions << "\n";
    }
    std::cout << "===================================================\n";
}

void MeasurementDataBase::reset()
{
    markers.clear();
}

MarkerInfo MeasurementDataBase::getMarkerInfo(const std::string_view name)
{
    return markers[std::string(name)];
}

Measurment::Measurment(const std::string_view n)
{
    name = n;
    starTime = std::chrono::steady_clock::now();
}

Measurment::~Measurment()
{
    auto endTime = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(endTime - starTime).count();
    MeasurementDataBase::update(name, duration);
}