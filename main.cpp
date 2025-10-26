#include "RoadMap.h"
#include "ShortestPath.h"
#include "AlternativeRoute.h"
#include "TrafficOptimization.h"
#include <iostream>
using namespace std;

int main() {
    RoadMap map;
    string file;
    cout << "Nhap ten file ban do (.txt): ";
    cin >> file;

    if (!map.loadFromFile(file)) {
        cerr << "Khong the tai ban do!\n";
        return 1;
    }

    cout << "\nBan do da tai thanh cong!\n";
    map.printMap();

    while (true) {
        cout << "\n================ MENU ================\n";
        cout << "1. Tim duong di ngan nhat\n";
        cout << "2. De xuat duong thay the khi gap su co\n";
        cout << "3. De xuat phuong an giam un tac\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "=====================================\n";
        cout << "Chon bai toan: ";

        int choice; cin >> choice;
        if (choice == 0) break;

        switch (choice) {
        case 1: {
            string start, goal;
            cout << "Nhap diem di: "; cin >> start;
            cout << "Nhap diem den: "; cin >> goal;
            ShortestPath sp(map);
            sp.findShortestPath(start, goal);
            break;
        }
        case 2: {
            string blocked, start, goal;
            cout << "Nhap ID duong bi su co: "; cin >> blocked;
            cout << "Nhap diem di: "; cin >> start;
            cout << "Nhap diem den: "; cin >> goal;
            AlternativeRoute ar(map);
            ar.suggestAlternative(blocked, start, goal);
            break;
        }
        case 3: {
            double budget;
            TrafficOptimization opt(map);
            opt.analyzeTraffic();
            cout << "Nhap ngan sach toi da (ty VND): ";
            cin >> budget;
            opt.proposeSolution(budget);
            break;
        }
        default:
            cout << "Lua chon khong hop le.\n";
        }
    }

    cout << "\nCam on ban da su dung he thong mo phong giao thong!\n";
    return 0;
}
