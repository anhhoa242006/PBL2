#include "TrafficOptimization.h"
#include "ShortestPath.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <set>
#include <unordered_map>
using namespace std;

// Named constants for traffic optimization calculations
namespace {
    const double NEW_ROAD_COST_MULTIPLIER = 1.75;  // New roads cost ~1.75x average
    const double DEFAULT_NEW_ROAD_COST = 70000.0;   // Default: 70,000 (units in tỷ VNĐ * 1000)
    const double INDIRECT_FLOW_REDIRECT_RATIO = 0.5; // 50% of indirect flow can redirect
    const double DIRECT_FLOW_REDIRECT_RATIO = 0.3;   // 30% of direct flow can redirect
    const double ESTIMATED_TIME_SAVINGS_MINUTES = 10.0; // Estimated travel time reduction
    const double BYPASS_FLOW_REDIRECT_RATIO = 0.4;   // 40% of flow can redirect through bypass
    const int DEFAULT_GREEN_LIGHT_TIME = 60;         // Default traffic light green time in seconds
}

TrafficOptimization::TrafficOptimization(RoadMap& map)
    : map_(map) {}

void TrafficOptimization::optimizeTraffic() {
    // Tự động phát hiện các tuyến đường bị ùn tắc
    cout << "\n🔍 Đang quét hệ thống để phát hiện các tuyến đường ùn tắc...\n";
    auto congestedRoads = detectCongestedRoads();
    
    if (congestedRoads.empty()) {
        cout << "\n✅ KHÔNG CÓ TUYẾN ĐƯỜNG NÀO BỊ QUÁ TẢI!\n";
        cout << "Tất cả các tuyến đường đang hoạt động bình thường (lưu lượng ≤ sức chứa).\n";
        return;
    }
    
    // Hiển thị danh sách các tuyến đường bị ùn tắc
    displayCongestedRoadsList(congestedRoads);
    
    // Người dùng chọn tuyến đường để phân tích
    string congestedEdgeId;
    double budget;
    
    cout << "\n📌 Nhập ID tuyến đường muốn phân tích (hoặc nhập 0 để hủy): ";
    cin >> congestedEdgeId;
    
    if (congestedEdgeId == "0") {
        cout << "Đã hủy phân tích.\n";
        return;
    }

    cout << "Nhập ngân sách tối đa (tỷ VNĐ): ";
    cin >> budget;

    if (!map_.hasEdge(congestedEdgeId)) {
        cout << "❌ Tuyến đường không tồn tại.\n";
        return;
    }
    
    // Lấy thông tin edge
    auto edgePtr = map_.getEdgeById(congestedEdgeId);
    if (!edgePtr) {
        cout << "❌ Không thể lấy thông tin tuyến đường.\n";
        return;
    }
    
    Edge& congestedEdge = *edgePtr;
    
    cout << "\n=== PHÂN TÍCH TÌNH TRẠNG ===\n";
    cout << "Tuyến đường ùn tắc: " << congestedEdge.id << " (" << congestedEdge.name << ")\n";
    cout << "Chiều: " << congestedEdge.src << " → " << congestedEdge.dst << "\n";
    cout << fixed << setprecision(0);
    cout << "Lưu lượng xe hiện tại: " << congestedEdge.flow << " xe/giờ\n";
    cout << "Sức chứa thiết kế: " << congestedEdge.capacity << " xe/giờ\n";
    cout << "Chi phí đường hiện tại: " << congestedEdge.budget << " tỷ VNĐ\n";
    cout << "Ngân sách dự kiến: " << budget << " tỷ VNĐ\n";
    cout << defaultfloat;
    
    // Tính mức độ quá tải
    if (congestedEdge.capacity > 0) {
        double congestionPercent = (congestedEdge.flow / congestedEdge.capacity) * 100;
        cout << fixed << setprecision(0);
        cout << "Mức độ quá tải: " << congestionPercent << "%\n";
        
        if (congestionPercent > 100) {
            cout << "⚠️  CẢNH BÁO: Tuyến đường đang quá tải " << (congestionPercent - 100) << "% so với thiết kế!\n";
        } else if (congestionPercent > 80) {
            cout << "⚠️  Tuyến đường đang trong tình trạng gần quá tải.\n";
        }
        cout << defaultfloat;
    } else {
        cout << "Mức độ quá tải: Không xác định (sức chứa = 0)\n";
    }

    // Tìm các phương án xây dựng tuyến đường mới
    auto proposals = findPotentialNewRoads(congestedEdge, budget);
    
    if (proposals.empty()) {
        // Tính ngân sách tối thiểu cần thiết
        double minBudgetNeeded = estimateMinimumBudget(congestedEdge);
        cout << "\n=== VẤN ĐÈ ===\n";
        cout << fixed << setprecision(0);
        cout << "⚠ Không tìm thấy giải pháp khả thi trong ngân sách " << budget << " tỷ VNĐ.\n";
        cout << "💰 Ngân sách tối thiểu cần thiết để xây tuyến đường mới: " 
             << minBudgetNeeded << " tỷ VNĐ\n";
        cout << "📊 Thiếu hụt ngân sách: " << (minBudgetNeeded - budget) << " tỷ VNĐ\n";
        cout << defaultfloat;
        
        cout << "\n=== GIẢI PHÁP THAY THẾ (KHÔNG CẦN NGÂN SÁCH) ===\n";
        displayTrafficSignalSolution(congestedEdge);
        return;
    }

    // Chọn phương án tốt nhất
    auto bestProposal = selectBestProposal(proposals);
    
    // Hiển thị giải pháp
    displayProposal(bestProposal, congestedEdge);
}

double TrafficOptimization::analyzeNodeCongestion(const string& nodeId) {
    double totalFlow = 0;
    double totalCapacity = 0;
    
    auto edges = map_.getEdges();
    for (const auto& e : edges) {
        if (e.dst == nodeId && !e.isReverse) {
            totalFlow += e.flow;
            totalCapacity += e.capacity;
        }
    }
    
    if (totalCapacity == 0) return 0;
    return totalFlow / totalCapacity;
}

vector<NewRoadProposal> TrafficOptimization::findPotentialNewRoads(const Edge& congestedEdge, double budget) {
    vector<NewRoadProposal> proposals;
    
    // Check if road is over capacity
    if (!isOverCapacity(congestedEdge)) {
        return proposals;
    }
    
    double roadCost = congestedEdge.budget;
    string srcNode = congestedEdge.src;
    string dstNode = congestedEdge.dst;
    
    // Requirement 1: If budget >= road cost, recommend building new road
    if (budget >= roadCost) {
        // Find bypass routes through intermediate nodes
        auto allNodes = map_.getNodeIds();
        auto edges = map_.getEdges();
        
        set<pair<string, string>> existingConnections;
        for (const auto& e : edges) {
            existingConnections.insert({e.src, e.dst});
            if (e.dir == Direction::TWO_WAY) {
                existingConnections.insert({e.dst, e.src});
            }
        }
        
        for (const string& intermediateNode : allNodes) {
            if (intermediateNode == srcNode || intermediateNode == dstNode) continue;
            
            bool canConnectFromSrc = existingConnections.count({srcNode, intermediateNode}) == 0;
            bool canConnectToDst = existingConnections.count({intermediateNode, dstNode}) == 0;
            
            if (canConnectFromSrc || canConnectToDst) {
                double estimatedCost = DEFAULT_NEW_ROAD_COST;
                double avgBudgetPerEdge = 0;
                int edgeCount = 0;
                
                for (const auto& e : edges) {
                    if (e.budget > 0 && !e.isReverse) {
                        avgBudgetPerEdge += e.budget;
                        edgeCount++;
                    }
                }
                
                if (edgeCount > 0) {
                    avgBudgetPerEdge = avgBudgetPerEdge / edgeCount;
                    estimatedCost = avgBudgetPerEdge * NEW_ROAD_COST_MULTIPLIER;
                }
                
                double potentialRedirectedFlow = congestedEdge.flow * BYPASS_FLOW_REDIRECT_RATIO;
                
                if (estimatedCost <= budget && potentialRedirectedFlow > 0) {
                    NewRoadProposal proposal;
                    proposal.type = ProposalType::NEW_ROAD;
                    
                    if (canConnectFromSrc && canConnectToDst) {
                        proposal.srcNode = srcNode;
                        proposal.dstNode = intermediateNode;
                        proposal.estimatedCost = estimatedCost * 2;
                        proposal.trafficReduction = potentialRedirectedFlow * 0.6;
                        proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES;
                        proposal.reasoning = "Ngân sách (" + to_string((int)budget) + " tỷ) >= chi phí đường (" 
                                           + to_string((int)roadCost) + " tỷ). Xây dựng tuyến đường vòng qua nút " 
                                           + intermediateNode + " để phân tán lưu lượng xe. Tạo 2 đoạn: " 
                                           + srcNode + "→" + intermediateNode + " và " + intermediateNode + "→" + dstNode + ".";
                        proposal.intermediateNode = intermediateNode;
                        proposal.isTwoSegment = true;
                    } else if (canConnectFromSrc) {
                        proposal.srcNode = srcNode;
                        proposal.dstNode = intermediateNode;
                        proposal.estimatedCost = estimatedCost;
                        proposal.trafficReduction = potentialRedirectedFlow * 0.3;
                        proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES * 0.7;
                        proposal.reasoning = "Ngân sách (" + to_string((int)budget) + " tỷ) >= chi phí đường (" 
                                           + to_string((int)roadCost) + " tỷ). Xây dựng tuyến đường từ " + srcNode 
                                           + " đến " + intermediateNode + " để tạo lối đi thay thế.";
                    } else {
                        proposal.srcNode = intermediateNode;
                        proposal.dstNode = dstNode;
                        proposal.estimatedCost = estimatedCost;
                        proposal.trafficReduction = potentialRedirectedFlow * 0.3;
                        proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES * 0.7;
                        proposal.reasoning = "Ngân sách (" + to_string((int)budget) + " tỷ) >= chi phí đường (" 
                                           + to_string((int)roadCost) + " tỷ). Xây dựng tuyến đường từ " 
                                           + intermediateNode + " đến " + dstNode + " để tạo lối đi thay thế.";
                    }
                    
                    if (proposal.estimatedCost <= budget) {
                        proposals.push_back(proposal);
                    }
                }
            }
        }
    }
    // Requirement 2: If budget is 1/3 to <1x road cost, recommend expanding lanes
    else if (budget >= roadCost / 3.0 && budget < roadCost) {
        auto expandProposal = createExpandLanesProposal(congestedEdge);
        // Keep the estimated cost from helper, but ensure it's within budget
        if (expandProposal.estimatedCost > budget) {
            expandProposal.estimatedCost = budget;
        }
        proposals.push_back(expandProposal);
    }
    
    // Requirement 3: Check for adjacent congested roads
    auto adjacentCongested = findAdjacentCongestedRoads(congestedEdge);
    if (adjacentCongested.size() >= 2) {  // At least 2 roads in sequence (including current)
        auto bypassProposal = createDirectBypassProposal(adjacentCongested, budget);
        if (bypassProposal.estimatedCost > 0 && bypassProposal.estimatedCost <= budget) {
            proposals.push_back(bypassProposal);
        }
    }
    
    return proposals;
}

NewRoadProposal TrafficOptimization::selectBestProposal(const vector<NewRoadProposal>& proposals) {
    if (proposals.empty()) {
        return NewRoadProposal();
    }
    
    // Chọn phương án có hiệu quả cao nhất (giảm tải nhiều nhất)
    NewRoadProposal best = proposals[0];
    double bestReduction = best.trafficReduction;
    
    for (const auto& p : proposals) {
        // Ưu tiên phương án giảm tải nhiều nhất trong ngân sách
        if (p.trafficReduction > bestReduction) {
            best = p;
            bestReduction = p.trafficReduction;
        }
    }
    
    return best;
}


void TrafficOptimization::displayProposal(const NewRoadProposal& proposal, const Edge& congestedEdge) {
    cout << "\n=== GIẢI PHÁP ĐỀ XUẤT ===\n";
    
    // Display based on proposal type
    if (proposal.type == ProposalType::EXPAND_LANES) {
        cout << "Phương án được chọn: Mở rộng làn đường\n";
        cout << "  - Tuyến đường: " << congestedEdge.id << " (" << proposal.srcNode << " → " << proposal.dstNode << ")\n";
        cout << "  - Loại: Mở rộng sức chứa hiện tại\n";
    } else if (proposal.type == ProposalType::DIRECT_BYPASS) {
        cout << "Phương án được chọn: Xây dựng đường nối thẳng\n";
        cout << "  - Chiều: " << proposal.srcNode << " → " << proposal.dstNode << "\n";
        cout << "  - Loại: Đường bypass cho chuỗi đường tắc liền kề\n";
        cout << "  - Các đường tắc được bypass: ";
        for (size_t i = 0; i < proposal.congestedPath.size(); ++i) {
            cout << proposal.congestedPath[i];
            if (i < proposal.congestedPath.size() - 1) cout << " → ";
        }
        cout << "\n";
    } else {  // NEW_ROAD
        if (proposal.isTwoSegment) {
            cout << "Phương án được chọn: Xây dựng tuyến đường vòng qua nút " << proposal.intermediateNode << "\n";
            cout << "  - Đoạn 1: " << proposal.srcNode << " → " << proposal.intermediateNode << "\n";
            cout << "  - Đoạn 2: " << proposal.intermediateNode << " → " << proposal.dstNode << "\n";
        } else {
            cout << "Phương án được chọn: Xây dựng tuyến đường mới\n";
            cout << "  - Chiều: " << proposal.srcNode << " → " << proposal.dstNode << "\n";
        }
    }
    
    cout << fixed << setprecision(0);
    cout << "Chi phí dự kiến: " << proposal.estimatedCost << " tỷ VNĐ\n";
    cout << defaultfloat;
    
    cout << "\n=== LÝ DO ===\n";
    cout << proposal.reasoning << "\n";
    
    cout << "\n=== PHÂN TÍCH HIỆU QUẢ ===\n";
    
    if (congestedEdge.flow > 0) {
        double reductionPercent = (proposal.trafficReduction / congestedEdge.flow) * 100;
        double newFlow = congestedEdge.flow - proposal.trafficReduction;
        double newCongestionPercent = (newFlow / congestedEdge.capacity) * 100;
        
        cout << fixed << setprecision(0);
        cout << "• Giảm lưu lượng trên tuyến " << congestedEdge.id << ":\n";
        cout << "  - Giảm " << reductionPercent << "% lưu lượng\n";
        cout << "  - Từ " << congestedEdge.flow << " xe/giờ xuống còn " << newFlow << " xe/giờ\n";
        cout << "  - Mức độ quá tải mới: " << newCongestionPercent << "%\n";
        cout << defaultfloat;
    }
    
    if (proposal.type == ProposalType::EXPAND_LANES) {
        cout << fixed << setprecision(0);
        cout << "• Thông số mở rộng đề xuất:\n";
        cout << "  - Tăng sức chứa thêm: " << proposal.trafficReduction << " xe/giờ (khoảng 40%)\n";
        cout << "  - Sức chứa mới: " << (congestedEdge.capacity + proposal.trafficReduction) << " xe/giờ\n";
        cout << "  - Loại: Thêm làn đường hoặc mở rộng mặt đường hiện tại\n";
        cout << defaultfloat;
    } else {
        cout << fixed << setprecision(0);
        cout << "• Thông số tuyến đường mới đề xuất:\n";
        cout << "  - Lưu lượng xe dự kiến: " << proposal.trafficReduction << " xe/giờ\n";
        cout << "  - Sức chứa thiết kế khuyến nghị: " << (proposal.trafficReduction * 1.3) << " xe/giờ\n";
        cout << defaultfloat;
        
        if (proposal.isTwoSegment) {
            cout << "  - Chiều đi: Hai chiều (TWO_WAY)\n";
            cout << "  - Loại đường: Đường chính (MAIN_ROAD)\n";
        } else {
            cout << "  - Chiều đi: Một chiều (ONE_WAY)\n";
            cout << "  - Loại đường: Đường chính (MAIN_ROAD)\n";
        }
    }
    
    cout << "\n• Lợi ích bổ sung:\n";
    cout << "  - Tăng độ linh hoạt mạng lưới giao thông\n";
    cout << fixed << setprecision(0);
    cout << "  - Giảm thời gian di chuyển trung bình " << proposal.travelTimeSaved << " phút\n";
    cout << defaultfloat;
    cout << "  - Giảm thiểu nguy cơ tắc nghẽn dây chuyền\n";
}

double TrafficOptimization::estimateMinimumBudget(const Edge& congestedEdge) {
    // Tính ngân sách trung bình của các edge hiện có
    double avgBudget = 0;
    int count = 0;
    auto edges = map_.getEdges();
    
    for (const auto& e : edges) {
        if (e.budget > 0 && !e.isReverse) {
            avgBudget += e.budget;
            count++;
        }
    }
    
    if (count > 0) {
        avgBudget = avgBudget / count;
        // Estimate based on the congested edge's characteristics
        // Longer/higher capacity roads need more budget
        double lengthFactor = congestedEdge.length > 0 ? congestedEdge.length / 3.0 : 1.0;
        return avgBudget * NEW_ROAD_COST_MULTIPLIER * lengthFactor;
    }
    
    return DEFAULT_NEW_ROAD_COST;
}

void TrafficOptimization::displayTrafficSignalSolution(const Edge& congestedEdge) {
    cout << "1. Điều chỉnh thời gian đèn tín hiệu:\n";
    cout << "   - Tăng thời gian đèn xanh cho hướng " << congestedEdge.src << " → " << congestedEdge.dst << "\n";
    
    // Tính toán thời gian đề xuất dựa trên mức độ quá tải
    if (congestedEdge.capacity > 0) {
        double congestionRatio = congestedEdge.flow / congestedEdge.capacity;
        int recommendedGreenTime = (int)(DEFAULT_GREEN_LIGHT_TIME * congestionRatio * 1.2);
        
        cout << "   - Thời gian đèn xanh đề xuất: " << recommendedGreenTime << " giây (hiện tại: " << DEFAULT_GREEN_LIGHT_TIME << " giây)\n";
    }
    
    cout << "\n2. Điều tiết luồng giao thông:\n";
    cout << "   - Hạn chế xe tải nặng vào giờ cao điểm (7-9h và 17-19h)\n";
    cout << "   - Khuyến khích phân làn theo loại phương tiện\n";
    
    cout << "\n3. Quản lý tốc độ:\n";
    cout << "   - Tốc độ tối đa khuyến nghị: " << (int)(congestedEdge.avgSpeed * 0.8) << " km/h (để tăng độ an toàn)\n";
    cout << "   - Đặt biển báo tốc độ điện tử thông minh\n";
    
    cout << "\n4. Giám sát và điều phối:\n";
    cout << "   - Lắp đặt camera giám sát lưu lượng xe\n";
    cout << "   - Triển khai hệ thống điều khiển tín hiệu thích ứng (Adaptive Traffic Control)\n";
    
    cout << "\n5. Thông tin và cảnh báo:\n";
    cout << "   - Cảnh báo tài xế về tình trạng tắc đường qua ứng dụng di động\n";
    cout << "   - Đề xuất tuyến đường thay thế cho người dân\n";
    
    cout << "\n⏱️  Thời gian triển khai: 2-4 tuần\n";
    cout << "💰 Chi phí ước tính: 5-10 tỷ VNĐ (chủ yếu cho thiết bị và công nghệ)\n";
}

bool TrafficOptimization::isOverCapacity(const Edge& edge) {
    return edge.flow > edge.capacity;
}

std::vector<std::string> TrafficOptimization::findAdjacentCongestedRoads(const Edge& startEdge) {
    std::vector<std::string> congestedPath;
    congestedPath.push_back(startEdge.id);
    
    // Find forward congested roads (from dst of current edge)
    std::string currentNode = startEdge.dst;
    auto edges = map_.getEdges();
    
    bool foundCongested = true;
    while (foundCongested) {
        foundCongested = false;
        for (const auto& e : edges) {
            if (e.src == currentNode && !e.isReverse && isOverCapacity(e)) {
                congestedPath.push_back(e.id);
                currentNode = e.dst;
                foundCongested = true;
                break;
            }
        }
    }
    
    // Find backward congested roads (from src of current edge)
    currentNode = startEdge.src;
    foundCongested = true;
    while (foundCongested) {
        foundCongested = false;
        for (const auto& e : edges) {
            if (e.dst == currentNode && !e.isReverse && isOverCapacity(e)) {
                congestedPath.insert(congestedPath.begin(), e.id);
                currentNode = e.src;
                foundCongested = true;
                break;
            }
        }
    }
    
    return congestedPath;
}

NewRoadProposal TrafficOptimization::createExpandLanesProposal(const Edge& congestedEdge) {
    NewRoadProposal proposal;
    proposal.type = ProposalType::EXPAND_LANES;
    proposal.srcNode = congestedEdge.src;
    proposal.dstNode = congestedEdge.dst;
    proposal.estimatedCost = congestedEdge.budget * 0.5;  // Expanding lanes costs ~50% of new road
    
    // Expanding lanes can increase capacity by 30-50%
    double capacityIncrease = congestedEdge.capacity * 0.4;
    proposal.trafficReduction = std::min(capacityIncrease, congestedEdge.flow - congestedEdge.capacity);
    proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES * 0.5;
    
    double budgetRatio = congestedEdge.budget > 0 ? (proposal.estimatedCost / congestedEdge.budget) : 0.5;
    proposal.reasoning = "Ngân sách (" + std::to_string((int)proposal.estimatedCost) 
                       + " tỷ) từ 1/3 đến nhỏ hơn chi phí đường (" 
                       + std::to_string((int)congestedEdge.budget) + " tỷ, tỷ lệ: " 
                       + std::to_string((int)(budgetRatio * 100)) 
                       + "%). Đề xuất mở rộng làn đường " + congestedEdge.id 
                       + " (" + congestedEdge.src + "→" + congestedEdge.dst 
                       + ") để tăng sức chứa khoảng 40%.";
    
    return proposal;
}

NewRoadProposal TrafficOptimization::createDirectBypassProposal(const std::vector<std::string>& congestedPath, double budget) {
    NewRoadProposal proposal;
    proposal.type = ProposalType::DIRECT_BYPASS;
    proposal.congestedPath = congestedPath;
    
    if (congestedPath.empty()) {
        proposal.estimatedCost = 0;
        return proposal;
    }
    
    // Calculate total cost of congested roads
    double totalCost = 0;
    double totalFlow = 0;
    auto edges = map_.getEdges();
    
    std::string firstNode, lastNode;
    for (const auto& edgeId : congestedPath) {
        for (const auto& e : edges) {
            if (e.id == edgeId && !e.isReverse) {
                totalCost += e.budget;
                totalFlow += e.flow;
                if (firstNode.empty()) {
                    firstNode = e.src;
                }
                lastNode = e.dst;
                break;
            }
        }
    }
    
    // Check if budget > 2/3 of total cost
    double thresholdCost = totalCost * 2.0 / 3.0;
    if (budget <= thresholdCost) {
        proposal.estimatedCost = 0;  // Signal that this proposal is not viable
        return proposal;
    }
    
    proposal.srcNode = firstNode;
    proposal.dstNode = lastNode;
    proposal.estimatedCost = totalCost * 0.8;  // Direct road costs ~80% of total
    proposal.trafficReduction = totalFlow * 0.5;  // Can redirect 50% of total flow
    proposal.travelTimeSaved = ESTIMATED_TIME_SAVINGS_MINUTES * congestedPath.size();
    
    std::string pathStr;
    for (size_t i = 0; i < congestedPath.size(); ++i) {
        pathStr += congestedPath[i];
        if (i < congestedPath.size() - 1) pathStr += "→";
    }
    
    proposal.reasoning = "Các đường liền kề (" + pathStr + ") đều bị tắc. "
                       + "Ngân sách (" + std::to_string((int)budget) 
                       + " tỷ) > 2/3 tổng chi phí (" + std::to_string((int)thresholdCost) 
                       + " tỷ). Đề xuất xây đường nối thẳng từ " + firstNode 
                       + " đến " + lastNode + " để giảm tải toàn bộ chuỗi đường tắc.";
    
    return proposal;
}

std::vector<CongestionInfo> TrafficOptimization::detectCongestedRoads() {
    std::vector<CongestionInfo> congestedRoads;
    auto edges = map_.getEdges();
    
    for (const auto& edge : edges) {
        // Chỉ kiểm tra các edge gốc (không phải edge ngược)
        if (edge.isReverse) {
            continue;
        }
        
        // Kiểm tra nếu lưu lượng vượt quá sức chứa
        if (edge.capacity > 0 && edge.flow > edge.capacity) {
            CongestionInfo info;
            info.edgeId = edge.id;
            info.edgeName = edge.name;
            info.srcNode = edge.src;
            info.dstNode = edge.dst;
            info.flow = edge.flow;
            info.capacity = edge.capacity;
            info.congestionRatio = edge.flow / edge.capacity;
            info.overloadPercent = ((edge.flow - edge.capacity) / edge.capacity) * 100.0;
            
            congestedRoads.push_back(info);
        }
    }
    
    // Sắp xếp theo mức độ quá tải giảm dần
    std::sort(congestedRoads.begin(), congestedRoads.end(), 
              [](const CongestionInfo& a, const CongestionInfo& b) {
                  return a.overloadPercent > b.overloadPercent;
              });
    
    return congestedRoads;
}

void TrafficOptimization::displayCongestedRoadsList(const std::vector<CongestionInfo>& congestedRoads) {
    cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    cout << "║           DANH SÁCH CÁC TUYẾN ĐƯỜNG BỊ ÙN TẮC              ║\n";
    cout << "╠════════════════════════════════════════════════════════════════╣\n";
    cout << "║  Tìm thấy " << congestedRoads.size() << " tuyến đường đang bị quá tải";
    // Tính toán padding động
    int numDigits = to_string(congestedRoads.size()).length();
    int padding = 64 - 28 - numDigits;  // 64 total - fixed text - number length
    cout << string(padding > 0 ? padding : 1, ' ') << "║\n";
    cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    int count = 1;
    for (const auto& info : congestedRoads) {
        cout << "┌────────────────────────────────────────────────────────────────┐\n";
        cout << "│ " << count++ << ". " << info.edgeId << " - " << info.edgeName << "\n";
        cout << "├────────────────────────────────────────────────────────────────┤\n";
        cout << "│ Chiều:          " << info.srcNode << " → " << info.dstNode << "\n";
        
        // Sử dụng fixed và setprecision thay vì cast sang int
        cout << fixed << setprecision(0);
        cout << "│ Lưu lượng:      " << info.flow << " xe/giờ\n";
        cout << "│ Sức chứa:       " << info.capacity << " xe/giờ\n";
        cout << "│ Tỷ lệ:          " << (info.congestionRatio * 100) << "%\n";
        
        // Hiển thị mức độ nghiêm trọng
        if (info.overloadPercent > 100) {
            cout << "│ Mức độ:         🔴 CỰC KỲ NGHIÊM TRỌNG (quá tải +" << info.overloadPercent << "%)\n";
        } else if (info.overloadPercent > 50) {
            cout << "│ Mức độ:         🟠 NGHIÊM TRỌNG (quá tải +" << info.overloadPercent << "%)\n";
        } else if (info.overloadPercent > 20) {
            cout << "│ Mức độ:         🟡 TRUNG BÌNH (quá tải +" << info.overloadPercent << "%)\n";
        } else {
            cout << "│ Mức độ:         🟢 NHẸ (quá tải +" << info.overloadPercent << "%)\n";
        }
        
        // Reset về default precision
        cout << defaultfloat;
        
        cout << "└────────────────────────────────────────────────────────────────┘\n";
        cout << "\n";
    }
}

std::vector<CongestionInfo> TrafficOptimization::getCongestedRoads() {
    return detectCongestedRoads();
}

TrafficOptimizationResult TrafficOptimization::analyzeCongestedRoad(const std::string& edgeId, double budget) {
    TrafficOptimizationResult result;
    result.hasCongestedRoads = false;
    result.hasProposal = false;
    result.budget = budget;
    result.minBudgetNeeded = 0;
    
    // Kiểm tra edge có tồn tại không
    if (!map_.hasEdge(edgeId)) {
        return result;
    }
    
    // Lấy thông tin edge
    auto edgePtr = map_.getEdgeById(edgeId);
    if (!edgePtr) {
        return result;
    }
    
    result.congestedEdge = *edgePtr;
    result.hasCongestedRoads = true;
    
    // Tìm các phương án
    auto proposals = findPotentialNewRoads(result.congestedEdge, budget);
    
    if (proposals.empty()) {
        // Tính ngân sách tối thiểu
        result.minBudgetNeeded = estimateMinimumBudget(result.congestedEdge);
        result.hasProposal = false;
        
        // Lấy giải pháp không cần ngân sách
        result.trafficSignalSolutions = getTrafficSignalSolutions(result. congestedEdge);
    } else {
        // Chọn phương án tốt nhất
        result.bestProposal = selectBestProposal(proposals);
        result.hasProposal = true;
    }
    
    return result;
}

std::vector<std::string> TrafficOptimization::getTrafficSignalSolutions(const Edge& congestedEdge) {
    std::vector<std::string> solutions;
    
    solutions.push_back("1. Điều chỉnh thời gian đèn tín hiệu:");
    solutions.push_back("   - Tăng thời gian đèn xanh cho hướng " + congestedEdge.src + " → " + congestedEdge.dst);
    
    if (congestedEdge.capacity > 0) {
        double congestionRatio = congestedEdge.flow / congestedEdge.capacity;
        int recommendedGreenTime = (int)(60 * congestionRatio * 1.2);  // DEFAULT_GREEN_LIGHT_TIME = 60
        
        solutions.push_back("   - Thời gian đèn xanh đề xuất: " + std::to_string(recommendedGreenTime) + " giây (hiện tại: 60 giây)");
    }
    
    solutions.push_back("");
    solutions.push_back("2. Điều tiết luồng giao thông:");
    solutions.push_back("   - Hạn chế xe tải nặng vào giờ cao điểm (7-9h và 17-19h)");
    solutions.push_back("   - Khuyến khích phân làn theo loại phương tiện");
    
    solutions.push_back("");
    solutions.push_back("3. Quản lý tốc độ:");
    solutions.push_back("   - Tốc độ tối đa khuyến nghị: " + std::to_string((int)(congestedEdge.avgSpeed * 0.8)) + " km/h");
    solutions.push_back("   - Đặt biển báo tốc độ điện tử thông minh");
    
    solutions.push_back("");
    solutions.push_back("4. Giám sát và điều phối:");
    solutions.push_back("   - Lắp đặt camera giám sát lưu lượng xe");
    solutions.push_back("   - Triển khai hệ thống điều khiển tín hiệu thích ứng");
    
    solutions.push_back("");
    solutions.push_back("Thời gian triển khai:  2-4 tuần");
    solutions.push_back("Chi phí ước tính: 5-10 tỷ VNĐ");
    
    return solutions;
}
