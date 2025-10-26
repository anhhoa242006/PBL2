#include "ShortestPath.h"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>
using namespace std;

struct NodeState {
    string id;
    double dist;
    bool operator>(const NodeState& other) const { return dist > other.dist; }
};

ShortestPath::ShortestPath(RoadMap& map) : map_(map) {}

void ShortestPath::findShortestPath(const string& start, const string& goal) {
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;

    for (auto& id : map_.getNodeIds()) dist[id] = numeric_limits<double>::infinity();
    dist[start] = 0.0;

    priority_queue<NodeState, vector<NodeState>, greater<NodeState>> pq;
    pq.push({start, 0.0});

    while (!pq.empty()) {
        auto [u, d] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;
        for (auto& e : map_.outgoingEdges(u)) {
            double nd = d + e.travelTime();
            if (nd < dist[e.dst]) {
                dist[e.dst] = nd;
                prev[e.dst] = u;
                pq.push({e.dst, nd});
            }
        }
    }

    if (dist[goal] == numeric_limits<double>::infinity()) {
        cout << "Khong tim thay duong tu " << start << " den " << goal << ".\n";
        return;
    }

    vector<string> path;
    for (string at = goal; at != ""; at = prev.count(at)?prev[at]:"") path.push_back(at);
    reverse(path.begin(), path.end());

    cout << "Tuyen ngan nhat: ";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " ";
    }
    cout << "\nChieu dai (thoi gian): " << dist[goal] << " don vi\n";
}
