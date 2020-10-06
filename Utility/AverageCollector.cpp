#include "AverageCollector.h"

AverageCollector::AverageCollector(bool limitStorageUsage) : limitStorageUsage_(limitStorageUsage) {}

AverageCollector::~AverageCollector() {
    for (std::pair<std::string, std::vector<double>> pair : durationsByName_) {
        const double average = std::accumulate(pair.second.begin(), pair.second.end(), 0.0) / pair.second.size();

        std::cout << pair.first << " averaged " << average << "\n\n";
    }
}

void AverageCollector::report(const std::string& name, const double value) {
#pragma omp critical
    {
        std::vector<double>& vec = durationsByName_[name];

        if (limitStorageUsage_ && vec.size() == maxSize_)
            vec.erase(vec.begin());

        vec.push_back(value);
    }
}