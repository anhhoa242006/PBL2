#pragma once
#include "RoadMap.h"
#include <string>
using namespace std;

class AlternativeRoute {
public:
    AlternativeRoute(RoadMap& map);
    void suggestAlternative(const string& blockedEdgeId,
                            const string& start,
                            const string& goal);

private:
    RoadMap& map_;
};
