// main_gui.cpp - GUI version of the traffic map application
#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "RoadMap.h"
#include "ShortestPath.h"
#include "AlternativeRoute.h"
#include "TrafficOptimization.h"
#include "GuiRenderer.h"

using namespace std;

// Menu states
enum MenuState {
    MENU_MAIN,
    MENU_SHORTEST_PATH,
    MENU_ALTERNATIVE_ROUTE,
    MENU_TRAFFIC_OPTIMIZATION,
    MENU_LOAD_MAP
};

// Input dialog for getting text input
string showInputDialog(GuiRenderer& gui, const string& prompt) {
    string input;
    bool done = false;
    
    while (!done) {
        SDL_Event event;
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                return "";
            }
            
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                    done = true;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && input.length() > 0) {
                    input.pop_back();
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return "";
                }
            }
            
            if (event.type == SDL_TEXTINPUT) {
                input += event.text.text;
            }
        }
        
        // Render input dialog
        gui.clear(Color(40, 40, 50));
        
        gui.drawPanel(200, 200, 600, 200, "Input");
        gui.drawText(prompt, 220, 250, Color(255, 255, 255));
        gui.drawText("> " + input + "_", 220, 300, Color(200, 200, 255));
        gui.drawText("Press ENTER to confirm, ESC to cancel", 220, 350, Color(150, 150, 150));
        
        gui.present();
        SDL_Delay(16);  // ~60 FPS
    }
    
    return input;
}

void showMessageDialog(GuiRenderer& gui, const string& title, const vector<string>& messages) {
    bool done = false;
    
    while (!done) {
        SDL_Event event;
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
            
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                done = true;
            }
        }
        
        gui.clear(Color(40, 40, 50));
        
        int panelHeight = 150 + messages.size() * 30;
        gui.drawPanel(150, 150, 700, panelHeight, title);
        
        int y = 200;
        for (const auto& msg : messages) {
            gui.drawText(msg, 170, y, Color(255, 255, 255));
            y += 30;
        }
        
        gui.drawText("Press any key or click to continue", 170, y + 20, Color(150, 150, 150));
        
        gui.present();
        SDL_Delay(16);
    }
}

void drawMainMenu(GuiRenderer& gui, RoadMap& map) {
    gui.clear(Color(40, 40, 50));
    
    // Draw title
    gui.drawTitle("HE THONG PHAN TICH BAN DO GIAO THONG");
    
    // Draw map panel
    gui.drawPanel(50, 80, 500, 500, "Ban Do Hien Tai");
    gui.drawMap(map, 80, 120, 1.0);
    
    // ===== THÊM LEGEND (CHÚ THÍCH MÀU) =====
    // Vẽ legend ở dưới bản đồ với cỡ chữ nhỏ và căn dọc
    int legendX = 70;
    int legendY = 510;  // Đưa lên cao hơn để vừa trong panel
    int lineSpacing = 18;  // Khoảng cách giữa các dòng (nhỏ hơn)
    
    gui.drawText("Do: Qua tai (>90%)", legendX, legendY, Color(220, 20, 20));
    gui.drawText("Cam: Gan qua tai (70-90%)", legendX, legendY + lineSpacing, Color(220, 160, 20));
    gui.drawText("Xanh: Binh thuong (<70%)", legendX, legendY + lineSpacing * 2, Color(60, 180, 60));
    
    // Draw info panel
    gui.drawPanel(570, 80, 400, 200, "Thong Tin Ban Do");
    int totalNodes = map.getNodeIds().size();
    int totalEdges = map.getEdges().size();
    
    gui.drawText("Tong Nodes:  " + to_string(totalNodes), 590, 120, Color(100, 255, 100));
    gui.drawText("Tong Edges: " + to_string(totalEdges), 590, 150, Color(100, 255, 100));
    
    // Draw menu buttons
    gui.clearButtons();
    gui.addButton(Button(570, 300, 400, 50, "1. Tim duong di ngan nhat", 1));
    gui.addButton(Button(570, 360, 400, 50, "2. Goi y tuyen duong thay the", 2));
    gui.addButton(Button(570, 420, 400, 50, "3. Phan tich toi uu hoa giao thong", 3));
    gui.addButton(Button(570, 480, 400, 50, "4. Tai them ban do tu file", 4));
    gui.addButton(Button(570, 540, 400, 50, "5. Thoat", 5));
    
    // Draw buttons
    for (size_t i = 0; i < 5; i++) {
        gui.drawButton(gui.buttons[i]);
    }
    
    gui.present();
}

void handleShortestPath(GuiRenderer& gui, RoadMap& map) {
    // Get start node
    string start = showInputDialog(gui, "Nhap ID Node Bat dau (Start):");
    if (start.empty()) return;
    
    // Get goal node
    string goal = showInputDialog(gui, "Nhap ID Node Ket thuc (Goal):");
    if (goal.empty()) return;
    
    // Check if nodes exist
    if (!map.hasNode(start)) {
        showMessageDialog(gui, "Loi", {"Node bat dau '" + start + "' khong ton tai."});
        return;
    }
    
    if (!map.hasNode(goal)) {
        showMessageDialog(gui, "Loi", {"Node ket thuc '" + goal + "' khong ton tai."});
        return;
    }
    
    // Find shortest path
    ShortestPath sp(map);
    vector<string> path;
    double time = sp.findShortestPath(start, goal, path);
    
    if (time < 0) {
        showMessageDialog(gui, "Ket qua", {"Khong tim thay duong di tu " + start + " den " + goal});
    } else {
        // Show result with visualization
        bool done = false;
        while (!done) {
            SDL_Event event;
            while (gui.pollEvent(event)) {
                if (event.type == SDL_QUIT) {
                    return;
                }
                if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                    done = true;
                }
            }
            
            gui.clear(Color(40, 40, 50));
            
            // Draw map with highlighted path
            gui.drawPanel(50, 80, 500, 500, "Duong di ngan nhat");
            gui.drawMap(map, 80, 120, 1.0);
            gui.highlightPath(map, path, 80, 120, 1.0);
            
            // Draw result panel
            gui.drawPanel(570, 80, 400, 500, "Ket qua");
            gui.drawText("Tuyen duong:", 590, 120, Color(255, 255, 100));
            
            int y = 150;
            string pathStr;
            for (size_t i = 0; i < path.size(); i++) {
                pathStr += path[i];
                if (i < path.size() - 1) pathStr += " -> ";
            }
            
            // Split long path into multiple lines
            int charsPerLine = 30;
            for (size_t i = 0; i < pathStr.length(); i += charsPerLine) {
                string line = pathStr.substr(i, charsPerLine);
                gui.drawText(line, 590, y, Color(255, 255, 255));
                y += 25;
            }
            
            gui.drawText("Thoi gian: " + to_string((int)(time * 60)) + " phut (" + 
                        to_string(time) + " gio)", 
                        590, y + 20, Color(100, 255, 100));
            
            gui.drawText("Press any key to continue", 590, 520, Color(150, 150, 150));
            
            gui.present();
            SDL_Delay(16);
        }
    }
}

void handleAlternativeRoute(GuiRenderer& gui, RoadMap& map) {
    // Get edge to block
    string edgeId = showInputDialog(gui, "Nhap ID Edge can chan:");
    if (edgeId.empty()) return;
    
    if (!map.hasEdge(edgeId)) {
        showMessageDialog(gui, "Loi", {"Edge '" + edgeId + "' khong ton tai."});
        return;
    }
    
    // Get start node
    string start = showInputDialog(gui, "Nhap ID Node Bat dau (Start):");
    if (start.empty()) return;
    
    // Get goal node
    string goal = showInputDialog(gui, "Nhap ID Node Ket thuc (Goal):");
    if (goal.empty()) return;
    
    if (!map.hasNode(start) || !map.hasNode(goal)) {
        showMessageDialog(gui, "Loi", {"Node bat dau hoac ket thuc khong ton tai."});
        return;
    }
    
    // Find alternative route - SỬ DỤNG PHƯƠNG THỨC MỚI
    AlternativeRoute alt(map);
    auto result = alt.findAlternativeRoute(edgeId, start, goal);
    
    if (! result.success) {
        showMessageDialog(gui, "Khong tim thay duong", {
            "Khong co tuyen duong thay the khi chan Edge " + edgeId,
            result.errorMessage
        });
        return;
    }
    
    // Hiển thị kết quả với visualization
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                done = true;
            }
        }
        
        gui.clear(Color(40, 40, 50));
        
        // Draw map with highlighted path
        gui.drawPanel(50, 80, 500, 500, "Tuyen duong thay the");
        gui.drawMap(map, 80, 120, 1.0);
        gui.highlightPath(map, result.path, 80, 120, 1.0);
        
        // Draw result panel
        gui.drawPanel(570, 80, 400, 500, "Ket qua");
        gui.drawText("Edge bi chan: " + result.blockedEdgeId, 590, 120, Color(255, 100, 100));
        
        gui.drawText("Tuyen duong thay the:", 590, 160, Color(255, 255, 100));
        
        int y = 190;
        string pathStr;
        for (size_t i = 0; i < result.path.size(); i++) {
            pathStr += result.path[i];
            if (i < result.path.size() - 1) pathStr += " -> ";
        }
        
        // Split long path into multiple lines
        int charsPerLine = 30;
        for (size_t i = 0; i < pathStr.length(); i += charsPerLine) {
            string line = pathStr.substr(i, charsPerLine);
            gui.drawText(line, 590, y, Color(255, 255, 255));
            y += 25;
        }
        
        gui.drawText("Thoi gian: " + to_string((int)(result.travelTime * 60)) + " phut (" + 
                    to_string(result.travelTime) + " gio)", 
                    590, y + 20, Color(100, 255, 100));
        
        gui.drawText("Nhan phim bat ky de tiep tuc", 590, 520, Color(150, 150, 150));
        
        gui.present();
        SDL_Delay(16);
    }
}

void handleTrafficOptimization(GuiRenderer& gui, RoadMap& map) {
    TrafficOptimization opt(map);
    
    // Lấy danh sách đường bị tắc
    auto congestedRoads = opt.getCongestedRoads();
    
    if (congestedRoads.empty()) {
        showMessageDialog(gui, "Thong bao", {
            "Khong co tuyen duong nao bi qua tai! ",
            "Tat ca cac tuyen duong dang hoat dong binh thuong."
        });
        return;
    }
    
    // Hiển thị danh sách đường tắc
    bool selectingRoad = true;
    int selectedIndex = -1;
    
    while (selectingRoad) {
        SDL_Event event;
        
        // Xử lý tất cả events trong một vòng lặp
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                return;
            }
            
            if (event.type == SDL_MOUSEMOTION) {
                gui.handleMouseMotion(event.motion.x, event.motion.y);
            }
            
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int btnId = gui.handleMouseClick(event.button.x, event.button.y);
                if (btnId >= 0 && btnId < (int)congestedRoads.size()) {
                    selectedIndex = btnId;
                    selectingRoad = false;
                }
            }
        }
        
        // Vẽ UI
        gui.clear(Color(40, 40, 50));
        gui.drawTitle("Cac tuyen duong bi un tac");
        
        int panelHeight = 100 + congestedRoads.size() * 80;
        gui.drawPanel(100, 80, 800, panelHeight, "Chon tuyen duong de phan tich");
        
        int y = 120;
        gui.clearButtons();
        
        for (size_t i = 0; i < congestedRoads.size(); i++) {
            const auto& info = congestedRoads[i];
            
            string btnText = info.edgeId + " (" + info.srcNode + "->" + info.dstNode + ") - Qua tai: " + 
                           to_string((int)info.overloadPercent) + "%";
            gui.addButton(Button(120, y, 760, 60, btnText, i));
            
            y += 70;
        }
        
        // Vẽ các nút
        for (size_t i = 0; i < gui.buttons.size(); i++) {
            gui.drawButton(gui.buttons[i]);
        }
        
        gui.present();
        SDL_Delay(16);
    }
    
    if (selectedIndex < 0) return;
    
    // Nhập ngân sách
    string budgetStr = showInputDialog(gui, "Nhap ngan sach (ty VND):");
    if (budgetStr.empty()) return;
    
    double budget = 0;
    try {
        budget = stod(budgetStr);
        if (budget <= 0) {
            showMessageDialog(gui, "Loi", {"Ngan sach phai lon hon 0"});
            return;
        }
    } catch (...) {
        showMessageDialog(gui, "Loi", {"Ngan sach khong hop le"});
        return;
    }
    
    // Phân tích
    auto result = opt.analyzeCongestedRoad(congestedRoads[selectedIndex].edgeId, budget);
    
    // Hiển thị kết quả
    bool done = false;
    int scrollOffset = 0;
    
    while (!done) {
        SDL_Event event;
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE || 
                    event.key.keysym.sym == SDLK_RETURN) {
                    done = true;
                }
                if (event.key.keysym.sym == SDLK_UP) scrollOffset -= 30;    // Tăng tốc độ scroll
                if (event.key.keysym. sym == SDLK_DOWN) scrollOffset += 30;  // Tăng tốc độ scroll
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                done = true;
            }
        }
        
        gui.clear(Color(40, 40, 50));
        gui.drawTitle("Ket qua phan tich toi uu hoa");
        
        // THAY ĐỔI 1:  Tăng chiều cao panel từ 500 -> 520
        gui.drawPanel(50, 80, 900, 520, "Giai phap de xuat");
        
        int y = 120 - scrollOffset;
        
        // Thông tin cơ bản
        gui.drawText("Tuyen duong:  " + result.congestedEdge.id, 70, y, Color(255, 255, 100));
        y += 20;  // GIẢM từ 25 -> 20
        
        gui.drawText("Chieu:   " + result.congestedEdge.src + " -> " + result.congestedEdge.dst, 
                    70, y, Color(255, 255, 255));
        y += 20;  // GIẢM từ 25 -> 20
        
        gui.drawText("Luu luong: " + to_string((int)result.congestedEdge.flow) + " xe/gio", 
                    70, y, Color(255, 255, 255));
        y += 20;  // GIẢM từ 25 -> 20
        
        gui.drawText("Suc chua: " + to_string((int)result.congestedEdge.capacity) + " xe/gio", 
                    70, y, Color(255, 255, 255));
        y += 20;  // GIẢM từ 25 -> 20
        
        gui. drawText("Ngan sach: " + to_string((int)budget) + " ty VND", 
                    70, y, Color(100, 255, 100));
        y += 30;  // GIẢM từ 40 -> 30
        
        if (result.hasProposal) {
            // Có phương án
            auto& proposal = result.bestProposal;
            
            gui.drawText("GIAI PHAP:", 70, y, Color(100, 255, 255));
            y += 25;  // GIẢM từ 30 -> 25
            
            // Loại phương án
            string typeStr;
            if (proposal.type == ProposalType::EXPAND_LANES) {
                typeStr = "Mo rong lan duong";
            } else if (proposal.type == ProposalType::DIRECT_BYPASS) {
                typeStr = "Xay duong noi thang";
            } else {
                typeStr = "Xay tuyen duong moi";
            }
            gui.drawText("Loai:  " + typeStr, 70, y, Color(255, 255, 255));
            y += 20;  // GIẢM từ 25 -> 20
            
            gui.drawText("Chi phi: " + to_string((int)proposal.estimatedCost) + " ty VND", 
                        70, y, Color(255, 255, 255));
            y += 20;  // GIẢM từ 25 -> 20
            
            gui. drawText("Giam tai: " + to_string((int)proposal.trafficReduction) + " xe/gio", 
                        70, y, Color(100, 255, 100));
            y += 20;  // GIẢM từ 25 -> 20
            
            gui.drawText("Tiet kiem thoi gian: " + to_string((int)proposal.travelTimeSaved) + " phut", 
                        70, y, Color(100, 255, 100));
            y += 25;  // GIẢM từ 30 -> 25
            
            // Lý do - chia thành nhiều dòng
            gui. drawText("LY DO:", 70, y, Color(255, 255, 100));
            y += 20;  // GIẢM từ 25 -> 20
            
            string reasoning = proposal.reasoning;
            int maxChars = 100;
            for (size_t i = 0; i < reasoning.length(); i += maxChars) {
                string line = reasoning.substr(i, maxChars);
                gui.drawText(line, 70, y, Color(200, 200, 200));
                y += 18;  // GIẢM từ 20 -> 18
            }
            
        } else {
            // Không có phương án trong ngân sách
            gui.drawText("KHONG CO GIAI PHAP KHA THI", 70, y, Color(255, 100, 100));
            y += 25;  // GIẢM từ 30 -> 25
            
            gui.drawText("Ngan sach toi thieu:  " + to_string((int)result.minBudgetNeeded) + " ty VND", 
                        70, y, Color(255, 255, 100));
            y += 20;  // GIẢM từ 25 -> 20
            
            gui.drawText("Thieu hut: " + to_string((int)(result.minBudgetNeeded - budget)) + " ty VND", 
                        70, y, Color(255, 100, 100));
            y += 30;  // GIẢM từ 40 -> 30
            
            // Giải pháp thay thế
            gui.drawText("GIAI PHAP THAY THE (khong can ngan sach):", 70, y, Color(100, 255, 255));
            y += 20;  // GIẢM từ 25 -> 20
            
            for (const auto& sol : result.trafficSignalSolutions) {
                gui.drawText(sol, 70, y, Color(200, 200, 200));
                y += 18;  // GIẢM từ 20 -> 18
            }
        }
        
        // THAY ĐỔI 2: Dời dòng hướng dẫn xuống
        gui.drawText("Nhan ESC hoac ENTER de dong, UP/DOWN de cuon", 70, 565, Color(150, 150, 150));
        
        gui.present();
        SDL_Delay(16);
    }
}


void handleLoadMap(GuiRenderer& gui, RoadMap& map) {
    string filename = showInputDialog(gui, "Nhap ten file ban do:");
    if (filename.empty()) return;
    
    if (map.loadFromFile(filename)) {
        showMessageDialog(gui, "Thanh cong", {
            "Tai ban do tu " + filename + " thanh cong!",
            "Tong " + to_string(map.getNodeIds().size()) + " nodes",
            "Tong " + to_string(map.getEdges().size()) + " edges"
        });
    } else {
        showMessageDialog(gui, "Loi", {
            "Khong the tai file: " + filename,
            "Kiem tra ten file va dinh dang."
        });
    }
}

int main(int argc, char* argv[]) {
    // Initialize GUI
    GuiRenderer gui;
    // THAY ĐỔI:  Tăng chiều cao từ 600 -> 700
    if (! gui.init("He Thong Phan Tich Ban Do Giao Thong", 1000, 700)) {
        cerr << "Failed to initialize GUI!" << endl;
        return 1;
    }
    
    // Initialize map
    RoadMap map;
    
    // Try to load map from command line argument or default file
    string mapFile = "map.txt";
    if (argc > 1) {
        mapFile = argv[1];
    }
    
    // Load map silently in GUI mode
    map.loadFromFile(mapFile);
    
    // Main event loop
    bool quit = false;
    MenuState state = MENU_MAIN;
    
    // Enable text input
    SDL_StartTextInput();
    
    while (!quit) {
        SDL_Event event;
        
        // Handle events
        while (gui.pollEvent(event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            
            if (state == MENU_MAIN) {
                if (event.type == SDL_MOUSEMOTION) {
                    gui.handleMouseMotion(event.motion.x, event.motion.y);
                }
                
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int buttonId = gui.handleMouseClick(event.button.x, event.button.y);
                    
                    switch (buttonId) {
                        case 1:
                            handleShortestPath(gui, map);
                            break;
                        case 2:
                            handleAlternativeRoute(gui, map);
                            break;
                        case 3:
                            handleTrafficOptimization(gui, map);
                            break;
                        case 4:
                            handleLoadMap(gui, map);
                            break;
                        case 5:
                            quit = true;
                            break;
                    }
                }
            }
        }
        
        // Render
        if (state == MENU_MAIN) {
            drawMainMenu(gui, map);
        }
        
        SDL_Delay(16);  // ~60 FPS
    }
    
    SDL_StopTextInput();
    
    return 0;
}
