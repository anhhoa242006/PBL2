#pragma once
#include "RoadMap.h"
#include <string>

class TrafficOptimization {
public:
    TrafficOptimization(RoadMap& map);

    void analyzeTraffic();             // Phân tích nút tắc nghẽn
    void proposeSolution(double budget); // Đề xuất giải pháp dựa vào ngân sách

private:
    RoadMap& map_;
    std::string mostCongestedNode_;
    double congestionLevel_; // giá trị từ 0.0 đến 1.0
};
