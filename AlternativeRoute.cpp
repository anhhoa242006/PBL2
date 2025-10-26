#include "AlternativeRoute.h"
#include "ShortestPath.h"
#include <iostream>
using namespace std;

AlternativeRoute::AlternativeRoute(RoadMap& map) : map_(map) {}

void AlternativeRoute::suggestAlternative(const string& blockedEdgeId,
                                          const string& start,
                                          const string& goal) {
    if (!map_.blockEdge(blockedEdgeId)) {
        cout << "Duong " << blockedEdgeId << " khong ton tai.\n";
        return;
    }

    cout << "Duong " << blockedEdgeId << " dang bi phong toa.\n";
    cout << "Dang tim tuyen thay the...\n";

    ShortestPath sp(map_);
    sp.findShortestPath(start, goal);

    map_.unblockAll();
}
