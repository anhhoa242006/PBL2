#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

// Forward decleration de class Edge co the tham chieu Node
class Node;

// Class dai dien cho mot con duong (Edge)
class Edge {
    private:
        string id;
        string name;
        double length;
        double avgSpeed;
        shared_ptr<Node> sourceNode;
        shared_ptr<Node> destNode;
    public:
    // Constructor
        Edge(string id, string name, double length, double avgSpeed, shared_ptr<Node> source, shared_ptr<Node> dest)
        : id(id), name(name), length(length), avgSpeed(avgSpeed), sourceNode(source), destNode(dest) {}

    // Getters
    string getId() {
        return id;
    }
    string getName() {
        return name;
    }
    double getLength() {
        return length;
    }
    double getAvgSpeed() {
        return avgSpeed;
    }
    shared_ptr<Node> getSourceNode() {
        return sourceNode;
    }
    shared_ptr<Node> getDestNode() {
        return destNode;
    }
};

class Node {
    private:
        string id;
        string name;
        double latitude;
        double longitude;
        // Danh sach cac canh di ra tu nut nay
        vector<shared_ptr<Edge>> outgoingEdges;
    public:
        // Constructor
        Node(string id, string name, double latitude, double longitude)
        : id(id), name(name), latitude(latitude), longitude(longitude) {}

        // Getters
        string getId() {
            return id;
        }
        string getName() {
            return name;
        }
        double getLatitude() {
            return latitude;
        }
        double getLongitude() {
            return longitude;
        }

        //Them mot canh di ra
        void addOutgoingEdge(shared_ptr<Edge> edge) {
            outgoingEdges.push_back(edge);
        }
};

// Class quan ly toan bo ban do
class RoadMap {
    private:
        vector<shared_ptr<Node>> nodes;
        vector<shared_ptr<Edge>> edges;
    public:
        // Them mot node vao ban do
        shared_ptr<Node> addNode(const std::string& id, const std::string& name, double latitude, double longitude) {
        shared_ptr<Node> newNode = std::make_shared<Node>(id, name, latitude, longitude);
        nodes.push_back(newNode);
        return newNode;
        }

        // Them mot Edge vaof ban do
        shared_ptr<Edge> addEdge(const string& id, const string& name, double length, double avgSpeed, shared_ptr<Node> source, shared_ptr<Node> dest) {
            shared_ptr<Edge> newEdge = make_shared<Edge>(id, name, length, avgSpeed, source, dest);
            edges.push_back(newEdge);
            source->addOutgoingEdge(newEdge);
            return newEdge;
        }

        // In thong tin ban do
        void printMap() {
            cout << "-----------------------------------------" << endl;
            cout << "      BAN DO GIAO THONG THANH PHO      " << endl;
            cout << "-----------------------------------------" << endl;

            cout << "Cac diem (Nodes):" << endl;
            for(auto& node : nodes) {
                cout << " - ID: " << node->getId() << " , Ten: " << node->getName()
                     << ", Tao do: (" << node->getLatitude() << ", " << node->getLongitude() << ")" << endl;
            }

            cout << "\nCac duong (Edges):" << endl;
            for(auto& edge : edges) {
                cout << " - ID: " << edge->getId() << ", Ten duong: " << edge->getName()
                     << ", Ket noi tu " << edge->getSourceNode()->getName() << " den " << edge->getDestNode()->getName()
                     << ", Dai: " << edge->getLength() << "km, Toc do TB: " << edge->getAvgSpeed() << "km/h" << endl;
            }
            cout << "-----------------------------------------" << endl;
        }
};

int main() {
    RoadMap daNangMap;

    // Them cac diem (Node) va luu tru con tro den chung
    shared_ptr<Node> glSgt = daNangMap.addNode("GL_SGT", "Cong Vien A", 16.0691, 108.2239);
    shared_ptr<Node> glCtb = daNangMap.addNode("GL_CTB", "Nha Hat B", 16.0628, 108.2325);
    shared_ptr<Node> ddHl = daNangMap.addNode("DD_HL", "Thap Truyen Hinh C", 16.0601, 108.22272);
    shared_ptr<Node> ddMkt = daNangMap.addNode("DD_MKT", "Trung Tam Thuong Mai D", 16.0692, 108.2255);
    shared_ptr<Node> ddMk = daNangMap.addNode("DD_MK", "Cong Vien E", 16.0606, 108.2292);

    // Them cac duong (Edge)
    daNangMap.addEdge("road_SGT_MKT", "Duong so 1", 0.5, 20.0, glSgt, ddMkt);
    daNangMap.addEdge("road_SGT_HL", "Duong so 2", 1.2, 30.0, glSgt, ddHl);
    daNangMap.addEdge("road_HL_CTB", "Duong so 3", 0.7, 25.0, ddHl, glCtb);
    daNangMap.addEdge("road_CTB_MK", "Duong so 4", 1.0, 35.0, glCtb, ddMk);
    daNangMap.addEdge("road_MKT_CTB", "Duong so 5", 2.0, 30.0, ddMkt, glCtb);

    // In ban do
    daNangMap.printMap();

    return 0;

}
