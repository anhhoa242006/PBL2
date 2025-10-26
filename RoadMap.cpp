#include "RoadMap.h"
#include <fstream>
#include <iostream>

using namespace std;

void RoadMap::clear() {
    nodes_.clear();
    edges_.clear();
    adj_.clear();
    edgeById_.clear();
    blockedEdges_.clear();
}

bool RoadMap::addNode(const string& id, const string& name, double lat, double lon) {
    if (nodes_.count(id)) return false;
    nodes_[id] = make_shared<Node>(id,name,lat,lon);
    return true;
}

bool RoadMap::addEdge(const string& id, const string& name, double length, double avgSpeed,
                      const string& src, const string& dst, Direction dir, RoadType type) {
    if (!nodes_.count(src) || !nodes_.count(dst)) return false;
    if (edgeById_.count(id)) return false;
    auto e = make_shared<Edge>();
    e->id = id; e->name = name; e->length = length; e->avgSpeed = avgSpeed;
    e->src = src; e->dst = dst; e->dir = dir; e->type = type; e->isReverse = false;
    edges_.push_back(e);
    adj_[src].push_back(e);
    edgeById_[id] = e;

    if (dir == Direction::TWO_WAY) {
        string revId = id + "_rev";
        auto r = make_shared<Edge>();
        r->id = revId; r->name = name; r->length = length; r->avgSpeed = avgSpeed;
        r->src = dst; r->dst = src; r->dir = dir; r->type = type; r->isReverse = true; r->originalId = id;
        edges_.push_back(r);
        adj_[dst].push_back(r);
        edgeById_[revId] = r;
    }
    return true;
}

bool RoadMap::loadFromFile(const string& filename) {
    ifstream f(filename);
    if (!f.is_open()) return false;
    clear();
    int n; if (!(f>>n)) return false;
    for (int i=0;i<n;++i) {
        string id,name; double la,lo;
        f >> id >> name >> la >> lo;
        addNode(id,name,la,lo);
    }
    int m; if (!(f>>m)) return false;
    for (int i=0;i<m;++i) {
        string id,name,src,dst; double len,spd; int dirInt,typeInt;
        f >> id >> name >> len >> spd >> src >> dst >> dirInt >> typeInt;
        addEdge(id,name,len,spd,src,dst, (dirInt==1?Direction::ONE_WAY:Direction::TWO_WAY), static_cast<RoadType>(typeInt));
    }
    return true;
}

bool RoadMap::saveToFile(const string& filename) const {
    ofstream f(filename);
    if (!f.is_open()) return false;
    f << nodes_.size() << "\n";
    for (auto &p : nodes_) f << p.first << " " << p.second->name << " " << p.second->lat << " " << p.second->lon << "\n";
    int cnt=0; for (auto &e: edges_) if (!e->isReverse) ++cnt;
    f << cnt << "\n";
    for (auto &e: edges_) {
        if (e->isReverse) continue;
        f << e->id << " " << e->name << " " << e->length << " " << e->avgSpeed << " "
          << e->src << " " << e->dst << " " << (e->dir==Direction::ONE_WAY?1:2) << " " << static_cast<int>(e->type) << "\n";
    }
    return true;
}

bool RoadMap::blockEdge(const string& edgeId) {
    auto it = edgeById_.find(edgeId);
    if (it == edgeById_.end()) return false;
    blockedEdges_.insert(edgeId);
    // block cả reverse nếu có
    string rev = edgeId + "_rev";
    if (edgeById_.count(rev)) blockedEdges_.insert(rev);
    return true;
}

bool RoadMap::unblockEdge(const string& edgeId) {
    blockedEdges_.erase(edgeId);
    return true;
}
void RoadMap::unblockAll() { blockedEdges_.clear(); }

bool RoadMap::hasNode(const string& id) const { return nodes_.count(id); }
vector<string> RoadMap::getNodeIds() const {
    vector<string> out;
    for (auto &p : nodes_) out.push_back(p.first);
    return out;
}
vector<Edge> RoadMap::getEdges() const {
    vector<Edge> out;
    for (auto &e : edges_) out.push_back(*e);
    return out;
}
vector<Edge> RoadMap::outgoingEdges(const string& nodeId) const {
    vector<Edge> out;
    if (!adj_.count(nodeId)) return out;
    for (auto &e : adj_.at(nodeId)) {
        if (blockedEdges_.count(e->id)) continue;
        out.push_back(*e);
    }
    return out;
}

void RoadMap::printMap() const {
    cout << "=== MAP INFO ===\n";
    cout << "Nodes: " << nodes_.size() << "\n";
    for (auto &p : nodes_)
        cout << " - " << p.first << " (" << p.second->name << ")\n";
    cout << "Edges: " << edges_.size() << "\n";
    for (auto &e : edges_) {
        cout << " - " << e->id << " : " << e->src << " -> " << e->dst
             << " | len=" << e->length << "km | spd=" << e->avgSpeed
             << "km/h | dir=" << (e->dir == Direction::ONE_WAY ? "ONE" : "TWO")
             << (e->isReverse ? " [rev]" : "")
             << (blockedEdges_.count(e->id) ? " [BLOCKED]" : "")
             << "\n";
    }
}

