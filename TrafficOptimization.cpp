#include "TrafficOptimization.h"
#include <iostream>
#include <limits>
#include <cmath>
using namespace std;

TrafficOptimization::TrafficOptimization(RoadMap& map)
    : map_(map), congestionLevel_(0.0) {}

void TrafficOptimization::analyzeTraffic() {
    cout << "Phan tich luu luong giao thong...\n";

    auto nodeIds = map_.getNodeIds();
    double worstRatio = -1.0;
    string worstNode;

    for (auto& id : nodeIds) {
        auto outgoing = map_.outgoingEdges(id);
        if (outgoing.empty()) continue;

        double totalSpeed = 0;
        for (auto& e : outgoing) totalSpeed += e.avgSpeed;
        double avgSpeed = totalSpeed / outgoing.size();

        double congestion = 1.0 - (avgSpeed / 60.0); // 60 km/h = tốc độ lý tưởng

        if (congestion > worstRatio) {
            worstRatio = congestion;
            worstNode = id;
        }
    }

    mostCongestedNode_ = worstNode;
    congestionLevel_ = max(0.0, min(1.0, worstRatio));

    cout << "Nut tac nghen nhat: " << mostCongestedNode_ << "\n";
    cout << "Muc do tac nghen: " << (congestionLevel_ * 100) << "%\n";
}

void TrafficOptimization::proposeSolution(double budget) {
    if (mostCongestedNode_.empty()) {
        cout << "Chua co du lieu phan tich. Hay gọi analyzeTraffic() truoc.\n";
        return;
    }

    cout << "\nNgan sach kha dung: " << budget << " ty VND\n";
    cout << "De xuat gia phap:\n";

    double costSmall = 50;   // cải thiện tín hiệu, tổ chức lại giao thông
    double costMedium = 200; // mở rộng mặt đường, thêm làn xe
    double costBig = 700;    // xây tuyến mới

    if (congestionLevel_ < 0.3) {
        cout << "Giao thong on dinh, khong can can thiep.\n";
    } 
    else if (congestionLevel_ < 0.6) {
        if (budget >= costMedium) {
            cout << "De xuat: Mo rong mat duong quanh diem " << mostCongestedNode_ << ".\n";
            cout << "   Chi phi uoc tinh: " << costMedium << " ty VND.\n";
            cout << "   Hieu qua giam tac nghen khoang 20%.\n";
        } 
        else if (budget >= costSmall) {
            cout << "De xuat: Toi uu den tin hieu quanh nut " << mostCongestedNode_ << ".\n";
            cout << "   Chi phi uoc tinh: " << costSmall << " ty VND.\n";
            cout << "   Hieu qua giam tac nghen khoang 10%.\n";
        } 
        else {
            cout << "Ngan sach khong du cho phuong an cai thien.\n";
        }
    } 
    else { // congestion >= 0.6
        if (budget >= costBig) {
            cout << "De xuat: Xay tuyen duong moi tranh nut " << mostCongestedNode_ << ".\n";
            cout << "   Chi phi uoc tinh: " << costBig << " ty VND.\n";
            cout << "   Hieu qua giam tac nghen 35-40%.\n";
        } 
        else if (budget >= costMedium) {
            cout << "De xuat: Mo rong duong và Cai thien luong giao thong nut " 
                 << mostCongestedNode_ << ".\n";
            cout << "   Chi phi uoc tinh: " << costMedium << " ty VND.\n";
            cout << "   Hieu qua giam 25%.\n";
        } 
        else if (budget >= costSmall) {
            cout << "De xuat: Dieu chinh den tin hieu và phan luong hop ly.\n";
            cout << "   Chi phi uoc tinh: " << costSmall << " ty VND.\n";
            cout << "   Hieu qua giam 10%.\n";
        } 
        else {
            cout << "Ngan sach thap, khong the cai thien tinh hinh.\n";
        }
    }
}
