#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

struct Node;
struct Edge;

enum class Direction { ONE_WAY = 1, TWO_WAY = 2 };
enum class RoadType { HIGHWAY=0, MAIN_ROAD=1, STREET=2, ALLEY=3, BRIDGE=4, TUNNEL=5 };

struct Node {
    std::string id;
    std::string name;
    double lat;
    double lon;
    Node() = default;
    Node(const std::string& i, const std::string& n, double la, double lo)
        : id(i), name(n), lat(la), lon(lo) {}
};

struct Edge {
    std::string id;
    std::string name;
    double length;
    double avgSpeed;
    std::string src;
    std::string dst;
    Direction dir;
    RoadType type;
    bool isReverse;
    std::string originalId; // if reverse
    Edge() = default;
    double travelTime() const { return (avgSpeed>0.0) ? (length/avgSpeed) : 1e9; }
};

class RoadMap {
public:
    RoadMap() = default;
    void clear();

    bool loadFromFile(const std::string& filename); // format described below
    bool saveToFile(const std::string& filename) const;

    bool addNode(const std::string& id, const std::string& name, double lat, double lon);
    bool addEdge(const std::string& id, const std::string& name, double length, double avgSpeed,
                 const std::string& src, const std::string& dst, Direction dir, RoadType type);

    bool blockEdge(const std::string& edgeId);
    bool unblockEdge(const std::string& edgeId);
    void unblockAll();

    bool hasNode(const std::string& id) const;
    std::vector<std::string> getNodeIds() const;
    std::vector<Edge> getEdges() const; // returns copy of edges
    std::vector<Edge> outgoingEdges(const std::string& nodeId) const;

    void printMap() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Edge>> edges_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Edge>>> adj_; // src -> edges
    std::unordered_map<std::string, std::shared_ptr<Edge>> edgeById_;
    std::unordered_set<std::string> blockedEdges_;
};
