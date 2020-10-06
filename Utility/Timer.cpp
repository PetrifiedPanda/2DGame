#include "Timer.h"

Timer::Timer(std::string name, AverageCollector& manager) : manager_(manager) {
    name_ = std::move(name);
    start_ = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {
    end_ = std::chrono::high_resolution_clock::now();
    duration_ = end_ - start_;
    const double ms = duration_.count() * 1000.0;

    manager_.report(name_, ms);
}