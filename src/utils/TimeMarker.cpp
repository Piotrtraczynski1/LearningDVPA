#include "utils/TimeMarker.hpp"

std::map<std::string, MarkerInfo> MeasurementDataBase::markers;

void MeasurementDataBase::update(const std::string &name, uint64_t duration)
{
    if (markers.find(name) == markers.end())
    {
        markers[name] = MarkerInfo{duration, 1};
    }
    else
    {
        auto &marker = markers[name];
        marker.time += duration;
        marker.executions++;
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
        std::cout << "Marker " << marker.first << ", total duration: " << minutes << "m:" << seconds
                  << "s:" << millis << "ms"
                  << " (" << marker.second.time << " [micro])"
                  << " , total executions: " << marker.second.executions << "\n";
    }
    std::cout << "===================================================\n";
}

Measurment::Measurment(std::string n)
{
    name = n;
    starTime = std::chrono::high_resolution_clock::now();
}

Measurment::~Measurment()
{
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(endTime - starTime).count();
    MeasurementDataBase::update(name, duration);
}