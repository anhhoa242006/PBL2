#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

using namespace std;

const int INF = numeric_limits<int>::max();
const int MAX_VERTICES = 1000; 

// Cấu trúc để lưu trữ thông tin về cạnh: đỉnh đích và trọng số
struct Edge {
    int to;
    int weight;
};

// Cấu trúc để sử dụng trong priority_queue: khoảng cách và đỉnh
struct State {
    int distance;
    int vertex;
    // So sánh để priority_queue sắp xếp theo khoảng cách tăng dần
    bool operator>(const State& other) const {
        return distance > other.distance;
    }
};

// Hàm tìm đường đi ngắn nhất bằng thuật toán Dijkstra
pair<vector<int>, vector<int>> dijkstra(int startNode, int numVertices, const vector<vector<Edge>>& adj) {
    vector<int> distances(numVertices, INF);
    vector<int> predecessors(numVertices, -1);
    priority_queue<State, vector<State>, greater<State>> pq;

    distances[startNode] = 0;
    pq.push({0, startNode});

    while (!pq.empty()) {
        int currentDist = pq.top().distance;
        int u = pq.top().vertex;
        pq.pop();

        if (currentDist > distances[u]) {
            continue;
        }

        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int weight = edge.weight;
            if (distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                predecessors[v] = u;
                pq.push({distances[v], v});
            }
        }
    }
    return {distances, predecessors};
}

// Hàm in đường đi
void printPathAndDescription(int startNode, int endNode, const vector<int>& predecessors) {
    if (predecessors[endNode] == -1 && endNode != startNode) {
        cout << "Khong co duong di tu " << startNode << " den " << endNode << endl;
        return;
    }
    
    vector<int> path;
    int current = endNode;
    while (current != -1) {
        path.push_back(current);
        current = predecessors[current];
    }
    reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i < path.size() - 1) {
            cout << " -> ";
        }
    }
    cout << endl;
}

int main() {
    int numVertices, numEdges;
    cin >> numVertices >> numEdges;

    if (numVertices > MAX_VERTICES) {
        cout << "Loi: So dinh vuot qua gioi han " << MAX_VERTICES << "." << endl;
        return 1;
    }

    if (numEdges < 0 || numEdges > 5 * numVertices) {
        cout << "Loi: So canh khong hop le (0 <= m <= 5n)." << endl;
        return 1;
    }

    vector<vector<Edge>> adj(numVertices);
    for (int i = 0; i < numEdges; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) {
            cout << "Loi: Dinh khong hop le." << endl;
            return 1;
        }
        if (w < 1 || w > 10000) {
            cout << "Loi: Trong so phai thuoc [1, 10000]." << endl;
            return 1;
        }
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    int startNode, endNode;
    cin >> startNode >> endNode;

    if (startNode < 0 || startNode >= numVertices || endNode < 0 || endNode >= numVertices) {
        cout << "Loi: Dinh bat dau hoac ket thuc khong hop le." << endl;
        return 1;
    }

    auto result = dijkstra(startNode, numVertices, adj);
    vector<int> distances = result.first;
    vector<int> predecessors = result.second;

    if (distances[endNode] == INF) {
        cout << "Khong co duong di tu " << startNode << " den " << endNode << "." << endl;
    } else {
        printPathAndDescription(startNode, endNode, predecessors);
        cout << distances[endNode] << endl;
    }
    
    return 0;
}
