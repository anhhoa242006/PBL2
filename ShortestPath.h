#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>

class ShortestPath {
public:
    ShortestPath(RoadMap& map);
    double findShortestPath(const std::string& start,
                            const std::string& goal,
                            std::vector<std::string>& outPath);

private:
    RoadMap& map_;
};
