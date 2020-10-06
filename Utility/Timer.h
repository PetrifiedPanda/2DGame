#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <utility>

#include "AverageCollector.h"

class Timer {
   public:
    Timer(std::string name, AverageCollector& manager);

    ~Timer();

   private:
    std::string name_;
    AverageCollector& manager_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_, end_;
    std::chrono::duration<double> duration_;
};