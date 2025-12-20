#include "RoadMap.h"
#include <fstream>
#include <iostream>
#include <sstream> // Thêm thư viện này nếu cần xử lý chuỗi phức tạp hơn trong tương lai

using namespace std;

/**
 * @brief Xóa toàn bộ dữ liệu bản đồ.
 */
void RoadMap::clear() {
    nodes_.clear();
    edges_.clear();
    adj_.clear();
    edgeById_.clear();
    blockedEdges_.clear();
}

/**
 * @brief Tải dữ liệu bản đồ từ file.
 */
bool RoadMap::loadFromFile(const string& filename) {
    ifstream f(filename);
    if (!f.is_open()) return false;

    // 1. Đọc Nodes
    int n;
    if (!(f >> n)) return false;

    for (int i = 0; i < n; i++) {
        string id, name;
        double la, lo;
        f >> id >> name >> la >> lo;
        // Kiểm tra và thêm node (để tránh trùng lặp)
        if (!nodes_.count(id))
            addNode(id, name, la, lo);
    }

    // 2. Đọc Edges
    int m;
    if (!(f >> m)) return false;

    for (int i = 0; i < m; i++) {
        string id, name, src, dst;
        double len, spd, cap, fl, bud;
        int dirInt, typeInt;

        f >> id >> name >> len >> spd >> cap >> fl >> bud
          >> src >> dst >> dirInt >> typeInt;

        if (!edgeById_.count(id)) {
            // Thêm Edge gốc
            addEdge(id, name, len, spd, src, dst,
                dirInt == 1 ? Direction::ONE_WAY : Direction::TWO_WAY,
                static_cast<RoadType>(typeInt));

            // Cập nhật các trường flow/capacity/budget được đọc từ file
            auto e = edgeById_[id];
            e->capacity = cap;
            e->flow = fl;
            e->budget = bud;
            
            // Nếu là TWO_WAY, Edge ngược cũng cần được cập nhật
            if (e->dir == Direction::TWO_WAY) {
                string revId = id + "_rev";
                if(edgeById_.count(revId)){
                    auto r = edgeById_[revId];
                    r->capacity = cap;
                    r->flow = fl;
                    r->budget = bud;
                }
            }
        }
    }

    return true;
}

/**
 * @brief Lưu dữ liệu bản đồ ra file.
 */
bool RoadMap::saveToFile(const string& filename) const {
    ofstream f(filename);
    if (!f.is_open()) return false;

    // 1. Lưu Nodes
    f << nodes_.size() << "\n";
    for (auto &p : nodes_)
        f << p.first << " " << p.second->name << " "
          << p.second->lat << " " << p.second->lon << "\n";

    // 2. Lưu Edges (chỉ lưu Edge gốc, không lưu Edge reverse)
    int cnt = 0;
    for (auto &e : edges_) if (!e->isReverse) cnt++;
    f << cnt << "\n";

    for (auto &e : edges_) {
        if (e->isReverse) continue;

        f << e->id << " " << e->name << " "
          << e->length << " "
          << e->avgSpeed << " "
          << e->capacity << " "
          << e->flow << " "
          << e->budget << " "
          << e->src << " "
          << e->dst << " "
          << (e->dir == Direction::ONE_WAY ? 1 : 2) << " "
          << static_cast<int>(e->type)
          << "\n";
    }

    return true;
}

/**
 * @brief Thêm một Node mới.
 */
bool RoadMap::addNode(const string& id, const string& name, double lat, double lon) {
    if (nodes_.count(id)) return false;
    nodes_[id] = make_shared<Node>(id, name, lat, lon);
    return true;
}

/**
 * @brief Thêm một Edge mới.
 */
bool RoadMap::addEdge(const string& id, const string& name,
                      double length, double avgSpeed,
                      const string& src, const string& dst,
                      Direction dir, RoadType type) {

    // Kiểm tra tính hợp lệ: Node nguồn/đích phải tồn tại, Edge ID chưa tồn tại
    if (!nodes_.count(src) || !nodes_.count(dst)) return false;
    if (edgeById_.count(id)) return false;

    // Tạo Edge gốc
    auto e = make_shared<Edge>();
    e->id = id;
    e->name = name;
    e->length = length;
    e->avgSpeed = avgSpeed;
    e->src = src;
    e->dst = dst;
    e->dir = dir;
    e->type = type;
    e->capacity = 1000; // Giá trị mặc định
    e->flow = 0;        // Giá trị mặc định
    e->budget = 0;      // Giá trị mặc định

    edges_.push_back(e);
    adj_[src].push_back(e);
    edgeById_[id] = e;

    // Nếu là đường hai chiều, tạo Edge ngược
    if (dir == Direction::TWO_WAY) {
        string rev = id + "_rev";
        auto r = make_shared<Edge>(*e); // Sao chép dữ liệu Edge gốc
        r->id = rev;
        r->src = dst; // Đảo ngược nguồn và đích
        r->dst = src;
        r->isReverse = true;
        // Các giá trị cap, flow, budget được giữ nguyên
        
        edges_.push_back(r);
        adj_[dst].push_back(r);
        edgeById_[rev] = r;
    }

    return true;
}

/**
 * @brief Chặn một Edge (làm cho nó không thể sử dụng trong tìm đường).
 */
bool RoadMap::blockEdge(const string& edgeId) {
    if (!edgeById_.count(edgeId)) return false;
    blockedEdges_.insert(edgeId);

    // Nếu là đường hai chiều, chặn luôn Edge ngược
    string rev = edgeId + "_rev";
    if (edgeById_.count(rev))
        blockedEdges_.insert(rev);

    return true;
}

/**
 * @brief Bỏ chặn một Edge.
 */
bool RoadMap::unblockEdge(const string& edgeId) {
    blockedEdges_.erase(edgeId);
    blockedEdges_.erase(edgeId + "_rev");
    return true;
}

/**
 * @brief Bỏ chặn tất cả các Edge.
 */
void RoadMap::unblockAll() {
    blockedEdges_.clear();
}

/**
 * @brief Kiểm tra sự tồn tại của Node.
 */
bool RoadMap::hasNode(const string& id) const {
    return nodes_.count(id);
}

/**
 * @brief ĐÃ BỔ SUNG: Kiểm tra sự tồn tại của Edge (Để fix lỗi biên dịch).
 */
bool RoadMap::hasEdge(const string& id) const {
    return edgeById_.count(id);
}


/**
 * @brief Trả về danh sách ID của tất cả các Node.
 */
vector<string> RoadMap::getNodeIds() const {
    vector<string> v;
    for (auto &p : nodes_) v.push_back(p.first);
    return v;
}

/**
 * @brief Trả về con trỏ đến Node theo ID.
 */
shared_ptr<Node> RoadMap::getNodeById(const string& id) const {
    if (nodes_.count(id)) {
        return nodes_.at(id);
    }
    return nullptr;
}

/**
 * @brief Trả về danh sách tất cả các Edge (bao gồm cả reverse edges).
 */
vector<Edge> RoadMap::getEdges() const {
    vector<Edge> v;
    for (auto &e : edges_) v.push_back(*e);
    return v;
}

/**
 * @brief Trả về danh sách các Edge đi ra từ một Node, ngoại trừ các Edge bị chặn.
 */
vector<Edge> RoadMap::outgoingEdges(const string& nodeId) const {
    vector<Edge> out;
    if (!adj_.count(nodeId)) return out;
    for (auto &e : adj_.at(nodeId)) {
        if (!blockedEdges_.count(e->id))
            out.push_back(*e);
    }
    return out;
}

/**
 * @brief Trả về con trỏ Edge theo ID.
 */
shared_ptr<Edge> RoadMap::getEdgeById(const string& id) {
    if (!edgeById_.count(id)) return nullptr;
    return edgeById_[id];
}

/**
 * @brief In thông tin bản đồ ra console.
 */
void RoadMap::printMap() const {
    cout << "\n=== MAP ===\n";
    for (auto &e : edges_) {
        cout << e->id << ": " << e->src << "->" << e->dst
              << " | cap=" << e->capacity
              << " | flow=" << e->flow
              << (e->isReverse ? " [rev]" : "")
              << "\n";
    }
}