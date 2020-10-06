#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <numeric>

class AverageCollector {
    std::unordered_map<std::string, std::vector<double>> durationsByName_;

    bool limitStorageUsage_;
    const unsigned int maxSize_ = 1000;

   public:
    AverageCollector(bool limitStorageUsage);

    ~AverageCollector();

    void report(const std::string& name, double value);
};