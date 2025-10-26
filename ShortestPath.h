#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

class ShortestPath {
public:
    ShortestPath(RoadMap& map);
    void findShortestPath(const string& start, const string& goal);

private:
    RoadMap& map_;
};
