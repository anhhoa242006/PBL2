#include "TrafficOptimization.h"
#include "ShortestPath.h" // Cần include để tìm đường thay thế
#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>
using namespace std;

// === HÀM GIẢ ĐỊNH MAX FLOW VÀ LƯU LƯỢNG THỰC TẾ ===
double calculateMaxFlow(RoadMap& map, const string& start, const string& goal) {
    // THAY THẾ BẰNG THUẬT TOÁN MAX FLOW (Edmonds-Karp/Dinic) TẠI ĐÂY.
    // Giả định Max Flow
    if (start == "A" && goal == "C") return 100.0;
    if (start == "C" && goal == "A") return 100.0;
    if (start == "B" && goal == "D") return 70.0;
    if (start == "D" && goal == "B") return 70.0;
    return 50.0; 
}

double getActualFlow(const string& start, const string& goal) {
    // Dữ liệu giả định: Lưu lượng xe thực tế
    if (start == "A" && goal == "C") return 120.0; // Vượt ngưỡng
    if (start == "C" && goal == "A") return 120.0; 
    if (start == "B" && goal == "D") return 65.0;  // Dưới ngưỡng
    if (start == "D" && goal == "B") return 65.0;  
    return 40.0;
}
// ======================================================

TrafficOptimization::TrafficOptimization(RoadMap& map)
    : map_(map), congestedSource_(""), congestedSink_(""), maxFlow_(0.0), actualFlow_(0.0) {}

bool TrafficOptimization::analyzeRouteCongestion(const string& start, const string& goal) {
    if (!map_.hasNode(start) || !map_.hasNode(goal)) {
        cout << "Loi: Diem di hoac diem den khong ton tai trong ban do.\n";
        return false;
    }
    
    congestedSource_ = start;
    congestedSink_ = goal;

    maxFlow_ = calculateMaxFlow(map_, start, goal); 
    actualFlow_ = getActualFlow(start, goal);
    
    cout << "\nPhan tich tuyen duong " << start << " -> " << goal << " trong gio cao diem:\n";
    cout << " - Luong Cuc dai (Capacity): " << maxFlow_ << " xe/thoi diem\n";
    cout << " - Luu luong Thuc te: " << actualFlow_ << " xe/thoi diem\n";

    if (actualFlow_ > maxFlow_) {
        cout << "=> **CANH BAO UN TAC NGHIÊM TRONG!** Luu luong thuc te vuot Max Flow (" 
             << (actualFlow_ - maxFlow_) << " xe)!\n";
        return true; 
    } else {
        cout << "=> Giao thong on dinh. Luu luong thuc te duoi muc Max Flow.\n";
        return false;
    }
}

void TrafficOptimization::proposeNewRouteSolution() {
    if (congestedSource_.empty()) {
        cout << "Khong co du lieu un tac de de xuat giai phap.\n";
        return;
    }
    
    // Nếu tuyến đường bị ùn tắc (actualFlow_ > maxFlow_), tìm đường đi ngắn nhất
    // làm tuyến đường thay thế tiềm năng.
    cout << "\n================ DE XUAT GIA PHAP TRANH UN TAC =================\n";
    
    // Giả định rằng ShortestPath (dựa trên thời gian di chuyển) sẽ cung cấp
    // tuyến đường thay thế tối ưu nhất.
    ShortestPath sp(map_);
    cout << "De xuat tuyen duong thay the toi uu:\n";
    sp.findShortestPath(congestedSource_, congestedSink_);
    
    cout << "==============================================================\n";
    cout << "=> **Luu y:** Tuyen duong thay the tren (ngan nhat/nhanh nhat) se giup **phan tan** luu luong xe.\n";
    cout << "=> De co giai phap chinh xac hon (tranh cac canh bao hoa Max Flow), can phai mo phong lai Max Flow sau khi loai bo cac canh da dat capacity.\n";
}