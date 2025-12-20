#pragma once
#include "RoadMap.h"
#include <string>
#include <vector>

struct AlternativeRouteResult {
    bool success;
    std::vector<std::string> path;
    double travelTime;
    std::string blockedEdgeId;
    std::string errorMessage;
};

class AlternativeRoute {
public: 
    AlternativeRoute(RoadMap& map);
    
    void suggestAlternative(const std::string& blockedEdgeId,
                            const std::string& start,
                            const std::string& goal);
    
    AlternativeRouteResult findAlternativeRoute(const std::string& blockedEdgeId,
                                                 const std::string& start,
                                                 const std::string& goal);
private:
    RoadMap& map_;
};