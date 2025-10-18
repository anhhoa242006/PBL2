#include <bits/stdc++.h>
#include <algorithm> // đảm bảo có remove_if
using namespace std;

struct Edge {
    string to;
    int weight;
};

map<string, vector<Edge>> graph;

void addEdge(const string &u, const string &v, int w) {
    graph[u].push_back({v, w});
    graph[v].push_back({u, w});
}

void blockEdge(const string &a, const string &b) {
    auto &edgesA = graph[a];
    edgesA.erase(std::remove_if(edgesA.begin(), edgesA.end(),
        [&](const Edge &e){ return e.to == b; }), edgesA.end());

    auto &edgesB = graph[b];
    edgesB.erase(std::remove_if(edgesB.begin(), edgesB.end(),
        [&](const Edge &e){ return e.to == a; }), edgesB.end());
}

void dijkstra(const string &start, const string &end) {
    const int INF = numeric_limits<int>::max();
    map<string, int> dist;
    map<string, string> prev;

    // Khởi tạo dist cho mọi nút có trong graph
    for (const auto &p : graph) dist[p.first] = INF;
    // Nếu start hoặc end chưa có trong graph, vẫn phải khởi tạo để tránh lỗi
    if (dist.find(start) == dist.end()) dist[start] = INF;
    if (dist.find(end) == dist.end()) dist[end] = INF;

    dist[start] = 0;

    // min-heap theo khoảng cách
    priority_queue<pair<int,string>, vector<pair<int,string>>, greater<pair<int,string>>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto cur = pq.top(); pq.pop();
        int d = cur.first;
        string u = cur.second;

        if (d > dist[u]) continue;

        for (const auto &edge : graph[u]) {
            const string &v = edge.to;
            int w = edge.weight;
            if (dist[u] == INF) continue; // an toàn
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[end] == INF) {
        cout << "KHONG TIM THAY DUONG THAY THE!\n";
        return;
    }

    // Truy vết đường đi
    vector<string> path;
    string at = end;
    while (true) {
        path.push_back(at);
        if (at == start) break;
        if (prev.find(at) == prev.end()) { // phòng trường hợp không có prev
            break;
        }
        at = prev[at];
    }
    reverse(path.begin(), path.end());

    cout << "TUYEN THAY THE LA: ";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " → ";
    }
    cout << "\nCHIEU DAI: " << dist[end] << " km\n";
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Nếu có tham số dòng lệnh thì đọc từ file, ngược lại đọc từ stdin
    istream *pin = &cin;
    ifstream fin;
    bool fromFile = false;
    if (argc > 1) {
        fin.open(argv[1]);
        if (!fin.is_open()) {
            cerr << "KHONG MO DUOC FILE: " << argv[1] << '\n';
            return 1;
        }
        pin = &fin;
        fromFile = true;
    }

    auto &in = *pin;

    int m;
    if (!fromFile) cout << "NHAP SO DOAN DUONG: ";
    if (!(in >> m)) {
        cerr << "LOI DOC SO DOAN DUONG.\n";
        return 1;
    }

    if (!fromFile) cout << "NHAP CAC DOAN DUONG (DIEM 1 DIEM 2 DOAN DUONG ):\n";
    for (int i = 0; i < m; ++i) {
        string u, v;
        int w;
        in >> u >> v >> w;
        addEdge(u, v, w);
    }

    string start, end;
    if (!fromFile) cout << "NHAP DIEM DI: ";
    in >> start;
    if (!fromFile) cout << "NHAP DIEM DEN: ";
    in >> end;

    int k;
    if (!fromFile) cout << "NHAP SO DOAN DUONG BI PHONG TOA: ";
    in >> k;
    if (!fromFile) cout << "NHAP CAC DOAN BI PHONG TOA (DIEM 1 DIEM 2):\n";
    for (int i = 0; i < k; ++i) {
        string a, b;
        in >> a >> b;
        if (!fromFile) cout << "DUONG " << a << "–" << b << " DANG BI PHONG TOA.\n";
        blockEdge(a, b);
    }

    if (!fromFile) cout << "\n=== KET QUA ===\n";
    dijkstra(start, end);

    if (fin.is_open()) fin.close();
    return 0;
}
