#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>

class TrafficOptimization {
private:
    RoadMap& map_;
    std::string congestedSource_; // Cặp điểm người dùng nhập
    std::string congestedSink_;
    double maxFlow_;                  
    double actualFlow_;               

public:
    TrafficOptimization(RoadMap& map);

    // Phân tích: So sánh Actual Flow vs Max Flow cho cặp điểm cụ thể
    bool analyzeRouteCongestion(const std::string& start, const std::string& goal); 

    // Đề xuất: Tìm đường đi thay thế (sử dụng ShortestPath)
    void proposeNewRouteSolution();
};