#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

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

    double capacity;    // sức chứa
    double flow;        // lưu lượng
    double budget;      // chi phí

    std::string src;
    std::string dst;

    Direction dir;
    RoadType type;

    bool isReverse = false;
    std::string originalId;

    Edge()
        : length(0), avgSpeed(0),
          capacity(1000), flow(0), budget(0),
          dir(Direction::ONE_WAY), type(RoadType::STREET) {}

    double travelTime() const {
        return avgSpeed > 0 ? length / avgSpeed : 1e9;
    }
};

class RoadMap {
public:
    RoadMap() = default;

    void clear();
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;

    bool addNode(const std::string& id, const std::string& name, double lat, double lon);
    bool addEdge(const std::string& id, const std::string& name,
                  double length, double avgSpeed,
                  const std::string& src, const std::string& dst,
                  Direction dir, RoadType type);

    bool blockEdge(const std::string& edgeId);
    bool unblockEdge(const std::string& edgeId);
    void unblockAll();

    // Phương thức kiểm tra Node đã có trong file header của bạn
    bool hasNode(const std::string& id) const; 
    
    // ĐÃ THÊM: Phương thức kiểm tra Edge (Để khắc phục lỗi biên dịch)
    bool hasEdge(const std::string& id) const; 

    std::vector<std::string> getNodeIds() const;
    
    // Get node information
    std::shared_ptr<Node> getNodeById(const std::string& id) const;

    std::vector<Edge> getEdges() const;
    std::vector<Edge> outgoingEdges(const std::string& nodeId) const;

    std::shared_ptr<Edge> getEdgeById(const std::string& id);

    void printMap() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Edge>> edges_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Edge>>> adj_;
    std::unordered_map<std::string, std::shared_ptr<Edge>> edgeById_;
    std::unordered_set<std::string> blockedEdges_;
};