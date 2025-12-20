// ShortestPath.cpp
#include "ShortestPath.h"
#include <queue>
#include <unordered_map>
#include <limits>
#include <iostream>
#include <algorithm>  // <-- cần thiết cho std::reverse

using namespace std;

ShortestPath::ShortestPath(RoadMap& map) : map_(map) {}

double ShortestPath::findShortestPath(const string& start,
                                      const string& goal,
                                      vector<string>& outPath) {

    auto INF = numeric_limits<double>::infinity();
    unordered_map<string, double> dist;
    unordered_map<string, string> parent;

    // khởi tạo
    for (auto &id : map_.getNodeIds())
        dist[id] = INF;

    if (!map_.hasNode(start) || !map_.hasNode(goal)) return -1;

    dist[start] = 0;

    priority_queue<pair<double, string>,
                   vector<pair<double,string>>,
                   greater<pair<double,string>>> pq;

    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;

        for (auto &e : map_.outgoingEdges(u)) {
            double w = e.travelTime();
            if (dist[e.dst] > dist[u] + w) {
                dist[e.dst] = dist[u] + w;
                parent[e.dst] = u;
                pq.push({dist[e.dst], e.dst});
            }
        }
    }

    if (dist[goal] == INF) return -1;

    // truy vết đường — an toàn nếu parent không có key
    outPath.clear();
    string cur = goal;
    while (cur != start) {
        outPath.push_back(cur);
        if (!parent.count(cur)) {
            // unexpected: không có đường đầy đủ, trả về lỗi
            outPath.clear();
            return -1;
        }
        cur = parent[cur];
    }
    outPath.push_back(start);

    // đảo ngược thứ tự để từ start -> goal
    std::reverse(outPath.begin(), outPath.end());

    return dist[goal];
}
