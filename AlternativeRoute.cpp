#include "AlternativeRoute.h"
#include "ShortestPath.h"
#include <iostream>

using namespace std;

AlternativeRoute::AlternativeRoute(RoadMap& map)
    : map_(map) {}

void AlternativeRoute::suggestAlternative(const string& blockedEdgeId,
                                          const string& start,
                                          const string& goal) {

    map_.blockEdge(blockedEdgeId);

    vector<string> path;
    ShortestPath sp(map_);
    double t = sp.findShortestPath(start, goal, path);

    if (t < 0) cout << "❌ Không có tuyến thay thế.\n";
    else {
        cout << "✔ Tuyến thay thế:  ";
        for (auto &p : path) cout << p << " ";
        cout << "\nThời gian: " << t << "\n";
    }

    map_.unblockAll();
}

AlternativeRouteResult AlternativeRoute::findAlternativeRoute(const string& blockedEdgeId,
                                                               const string& start,
                                                               const string& goal) {
    AlternativeRouteResult result;
    result.blockedEdgeId = blockedEdgeId;
    result.success = false;
    result.travelTime = -1;
    
    // Chặn edge
    if (! map_.blockEdge(blockedEdgeId)) {
        result.errorMessage = "Không thể chặn edge " + blockedEdgeId;
        return result;
    }
    
    // Tìm đường thay thế
    ShortestPath sp(map_);
    double t = sp.findShortestPath(start, goal, result.path);
    
    // Bỏ chặn edge
    map_.unblockAll();
    
    if (t < 0) {
        result.success = false;
        result.errorMessage = "Không tìm thấy tuyến đường thay thế";
    } else {
        result.success = true;
        result.travelTime = t;
    }
    
    return result;
}