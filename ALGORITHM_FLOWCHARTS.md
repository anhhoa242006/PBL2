# LƯU ĐỒ THUẬT TOÁN - ALGORITHM FLOWCHARTS

## Mục Lục
1. [Lưu Đồ Thuật Toán Dijkstra](#1-lưu-đồ-thuật-toán-dijkstra)
2. [Lưu Đồ Tìm Đường Thay Thế](#2-lưu-đồ-tìm-đường-thay-thế)
3. [Lưu Đồ Phát Hiện Ùn Tắc](#3-lưu-đồ-phát-hiện-ùn-tắc)
4. [Lưu Đồ Tối Ưu Hóa Giao Thông](#4-lưu-đồ-tối-ưu-hóa-giao-thông)
5. [Lưu Đồ Logic Đề Xuất 3 Cấp](#5-lưu-đồ-logic-đề-xuất-3-cấp)
6. [Lưu Đồ Tìm Đường Vòng](#6-lưu-đồ-tìm-đường-vòng)
7. [Lưu Đồ Chọn Phương Án Tốt Nhất](#7-lưu-đồ-chọn-phương-án-tốt-nhất)

---

## 1. Lưu Đồ Thuật Toán Dijkstra
**File**: ShortestPath.cpp  
**Mục đích**: Tìm đường đi ngắn nhất từ start đến goal

```
START
  │
  ▼
┌─────────────────────┐
│ Khởi tạo:           │
│ dist[v] = ∞ ∀v      │
│ dist[start] = 0     │
│ parent = {}         │
│ pq = min-heap       │
└────────┬────────────┘
         │
         ▼
┌─────────────────────┐
│ start, goal tồn tại?│
└────────┬────────────┘
         │
    ┌────┴────┐
   NO        YES
    │          │
    ▼          ▼
 Return -1  pq.push(0, start)
              │
              ▼
         ┌─────────────┐
         │ pq rỗng?    │←─────┐
         └─┬───────────┘      │
           │                  │
      ┌────┴────┐            │
     YES       NO             │
      │         │             │
      │         ▼             │
      │    (d,u)=pq.pop()    │
      │         │             │
      │         ▼             │
      │    ┌─────────┐        │
      │    │d>dist[u]│        │
      │    └─┬───────┘        │
      │      │                │
      │  ┌───┴───┐           │
      │ YES     NO            │
      │  │       │            │
      │  └───────┼────────────┘
      │          │
      │          ▼
      │   For each (u,v):
      │     w = travelTime
      │          │
      │          ▼
      │   ┌──────────────┐
      │   │dist[v]>      │
      │   │dist[u]+w?    │
      │   └─┬────────────┘
      │     │
      │  ┌──┴──┐
      │ YES   NO
      │  │     │
      │  ▼     │
      │ Update │
      │ dist[v]│
      │ parent │
      │ pq.push│
      │  │     │
      │  └─────┴─────────────┐
      │                      │
      ▼                      │
 ┌──────────┐               │
 │dist[goal]│               │
 │  = ∞?    │               │
 └─┬────────┘               │
   │                        │
┌──┴──┐                     │
YES  NO                     │
│     │                     │
│     ▼                     │
│  Truy vết path           │
│  Reverse path            │
│     │                     │
└─────┴─────────────────────┘
      │
      ▼
 Return result
      │
      ▼
     END
```

**Giải thích**:
- **Priority Queue**: Luôn chọn node có khoảng cách nhỏ nhất
- **Relaxation**: Cập nhật dist[v] nếu tìm được đường ngắn hơn
- **Truy vết**: Dùng parent[] để xây dựng đường đi

**Độ phức tạp**: O((V+E) log V)

---

## 2. Lưu Đồ Tìm Đường Thay Thế
**File**: AlternativeRoute.cpp  

```
START
  │
  ▼
┌─────────────────────┐
│ Input:              │
│ - blockedEdgeId     │
│ - start, goal       │
└────────┬────────────┘
         │
         ▼
┌─────────────────────┐
│ map.blockEdge(id)   │
└────────┬────────────┘
         │
         ▼
┌─────────────────────┐
│ Dijkstra(start,goal)│
│ (bỏ qua blocked)    │
└────────┬────────────┘
         │
         ▼
┌─────────────────────┐
│ Tìm thấy đường?     │
└────────┬────────────┘
         │
    ┌────┴────┐
   NO        YES
    │          │
    ▼          ▼
 "Không có  "Tuyến thay thế:"
  đường"     path + time
    │          │
    └────┬─────┘
         │
         ▼
┌─────────────────────┐
│ map.unblockAll()    │
└────────┬────────────┘
         │
         ▼
        END
```

**Đặc điểm**:
- Đơn giản, tái sử dụng Dijkstra
- Đảm bảo unblock sau khi tìm kiếm

---

## 3. Lưu Đồ Phát Hiện Ùn Tắc
**Function**: detectCongestedRoads()

```
START
  │
  ▼
congestedRoads = []
  │
  ▼
┌─────────────────┐
│ For each edge   │←────┐
└────────┬────────┘     │
         │              │
         ▼              │
┌─────────────────┐     │
│ flow > capacity?│     │
└────────┬────────┘     │
         │              │
    ┌────┴────┐         │
   NO        YES        │
    │          │        │
    │          ▼        │
    │   ┌──────────────┐│
    │   │ Tạo info:    ││
    │   │ - congRatio  ││
    │   │ - overload%  ││
    │   └──────┬───────┘│
    │          │        │
    │          ▼        │
    │   Add to list    │
    │          │        │
    └──────────┼────────┘
               │
        Còn edges?──────┘
               │
               ▼
┌──────────────────────┐
│ Sort by congRatio    │
│ (giảm dần)           │
└──────────┬───────────┘
           │
           ▼
   Return congestedRoads
           │
           ▼
          END
```

**Output**: Danh sách đường tắc, sắp xếp theo mức độ nghiêm trọng

---

## 4. Lưu Đồ Tối Ưu Hóa Giao Thông
**Function**: optimizeTraffic()

```
START
  │
  ▼
Phát hiện ùn tắc
  │
  ▼
┌──────────────┐
│ Có đường tắc?│
└──────┬───────┘
       │
  ┌────┴────┐
 NO        YES
  │          │
  ▼          ▼
"Không có  Hiển thị danh sách
 ùn tắc"      │
  │           ▼
  │      Input edgeId, budget
  │           │
  │           ▼
  │      ┌─────────────┐
  │      │ Edge tồn tại?│
  │      └──────┬──────┘
  │             │
  │        ┌────┴────┐
  │       NO        YES
  │        │          │
  │        ▼          ▼
  │     "Không    Hiển thị thông tin
  │      tồn tại"   flow, capacity
  │        │          │
  │        │          ▼
  │        │    findPotentialNewRoads
  │        │          │
  │        │          ▼
  │        │    ┌──────────────┐
  │        │    │ Có proposals?│
  │        │    └──────┬───────┘
  │        │           │
  │        │      ┌────┴────┐
  │        │     NO        YES
  │        │      │          │
  │        │      ▼          ▼
  │        │   Tính min   selectBest
  │        │   budget       │
  │        │      │          ▼
  │        │      ▼       Display
  │        │   Hiển thị  proposal
  │        │   giải pháp    │
  │        │   đèn tín hiệu │
  │        │      │          │
  └────────┴──────┴──────────┘
           │
           ▼
          END
```

---

## 5. Lưu Đồ Logic Đề Xuất 3 Cấp
**Function**: findPotentialNewRoads()

```
START
  │
  ▼
┌──────────────────┐
│ Input:           │
│ - congestedEdge  │
│ - budget         │
└────────┬─────────┘
         │
         ▼
proposals = []
roadCost = edge.budget
         │
         ▼
┌──────────────────┐
│ flow > capacity? │
└────────┬─────────┘
         │
    ┌────┴────┐
   NO        YES
    │          │
    ▼          ▼
 Return []  So sánh budget
              │
              ▼
        ┌─────────────────────┐
        │ DECISION TREE       │
        └─────────┬───────────┘
                  │
     ┌────────────┼────────────┐
     │            │            │
     ▼            ▼            ▼
  CẤP 1        CẤP 2        CẤP 3
budget≥cost  cost/3≤      budget<
             budget<cost   cost/3
     │            │            │
     ▼            ▼            ▼
┌─────────┐  ┌─────────┐  ┌─────────┐
│ XÂY MỚI │  │ MỞ RỘNG │  │  ĐÈN    │
│         │  │  LÀN    │  │ TÍN HIỆU│
└────┬────┘  └────┬────┘  └────┬────┘
     │            │            │
     ▼            │            ▼
A. Tìm bypass    │        Return []
   qua inter-    │
   mediate       │
     │            │
     ▼            │
For each node    │
  path1: src→i   │
  path2: i→dst   │
     │            │
     ▼            │
Cả 2 tồn tại?    │
     │            │
  ┌──┴──┐         │
 NO    YES        │
  │     │         │
  │     ▼         │
  │  Create       │
  │  proposal     │
  │  NEW_ROAD     │
  │     │         │
  │     ▼         │
B. Tìm adjacent  │
   congested     │
     │            │
     ▼            │
adjacent.len≥2?  │
     │            │
  ┌──┴──┐         │
 NO    YES        │
  │     │         │
  │     ▼         │
  │  budget>      │
  │  2/3×total?   │
  │     │         │
  │  ┌──┴──┐      │
  │ NO    YES     │
  │  │     │      │
  │  │     ▼      │
  │  │  Create    │
  │  │  DIRECT_   │
  │  │  BYPASS    │
  │  │     │      │
  └──┴─────┼──────┘
           │
           ▼
      Create EXPAND_LANES
           │
           ▼
    Return proposals
           │
           ▼
          END
```

**3 Cấp Đề Xuất**:

| Cấp | Điều kiện | Giải pháp | Hiệu quả |
|-----|-----------|-----------|----------|
| 1 | budget ≥ cost | Xây mới | Giảm 30-60% |
| 1+ | budget > 2/3×total | Nối thẳng | Giảm 50% |
| 2 | cost/3 ≤ budget < cost | Mở rộng | +40% sức chứa |
| 3 | budget < cost/3 | Đèn tín hiệu | Tối ưu luồng |

---

## 6. Lưu Đồ Tìm Đường Vòng (Bypass)

```
START
  │
  ▼
For each intermediate
node in map
  │
  ▼
┌──────────────────┐
│ node = src OR    │
│ node = dst?      │
└────────┬─────────┘
         │
    ┌────┴────┐
   YES       NO
    │         │
 Skip         ▼
              │
    ┌─────────────────┐
    │ Dijkstra:       │
    │ path1=src→inter │
    └────────┬────────┘
             │
             ▼
    ┌─────────────────┐
    │ path1 tồn tại?  │
    └────────┬────────┘
             │
        ┌────┴────┐
       NO        YES
        │         │
     Skip         ▼
              ┌─────────────────┐
              │ Dijkstra:       │
              │ path2=inter→dst │
              └────────┬────────┘
                       │
                       ▼
              ┌─────────────────┐
              │ path2 tồn tại?  │
              └────────┬────────┘
                       │
                  ┌────┴────┐
                 NO        YES
                  │         │
               Skip         ▼
                        ┌────────────┐
                        │ Tính cost: │
                        │ avg×1.75   │
                        └─────┬──────┘
                              │
                              ▼
                        ┌────────────┐
                        │ Ước tính   │
                        │ traffic    │
                        │ reduction  │
                        └─────┬──────┘
                              │
                              ▼
                        Create proposal
                              │
                              ▼
                        Add to proposals
                              │
                              │
                      Next node ←──────┘
                              │
                              ▼
                        Return proposals
                              │
                              ▼
                             END
```

**Ví dụ**: A→B tắc, tìm C: A→C→B làm đường vòng

---

## 7. Lưu Đồ Chọn Phương Án Tốt Nhất

```
START
  │
  ▼
┌──────────────┐
│ proposals    │
│ rỗng?        │
└──────┬───────┘
       │
  ┌────┴────┐
 YES       NO
  │         │
  ▼         ▼
Return   bestProposal=first
 null    bestScore=-∞
  │         │
  │         ▼
  │    For each proposal
  │         │
  │         ▼
  │    ┌─────────────────┐
  │    │ Tính score:     │
  │    │                 │
  │    │ score =         │
  │    │ (traffic%×100)  │
  │    │ ─────────────   │
  │    │     cost        │
  │    │                 │
  │    │ + timeSaved     │
  │    └────────┬────────┘
  │             │
  │             ▼
  │    ┌─────────────────┐
  │    │ score >         │
  │    │ bestScore?      │
  │    └────────┬────────┘
  │             │
  │        ┌────┴────┐
  │       YES       NO
  │        │         │
  │        ▼         │
  │    Update best   │
  │        │         │
  │        └────┬────┘
  │             │
  │      Next proposal
  │             │
  └─────────────┼───┘
                │
                ▼
         Return bestProposal
                │
                ▼
               END
```

**Công thức**: 
```
score = (trafficReduction% × 100) / cost + timeSaved
```

---

## 8. Lưu Đồ Tìm Đường Liền Kề Tắc

```
START
  │
  ▼
┌──────────────────┐
│ Input: startEdge │
└────────┬─────────┘
         │
         ▼
path = [start.id]
currentNode = start.dst
visited = {start.id}
         │
         ▼
┌──────────────┐
│ While true   │←──────┐
└──────┬───────┘       │
       │               │
       ▼               │
   found=false        │
       │               │
       ▼               │
For each edge from    │
currentNode           │
       │               │
       ▼               │
┌──────────────┐       │
│ edge visited?│       │
└──────┬───────┘       │
       │               │
  ┌────┴────┐          │
 YES       NO          │
  │         │          │
Skip        ▼          │
       ┌──────────┐    │
       │ Over     │    │
       │ capacity?│    │
       └─────┬────┘    │
             │         │
        ┌────┴────┐    │
       NO        YES   │
        │         │    │
     Skip         ▼    │
           ┌──────────┐│
           │ Add path ││
           │ Update   ││
           │ found=YES││
           │ Break    ││
           └─────┬────┘│
                 │     │
          Next edge    │
                 │     │
                 ▼     │
         ┌──────────┐  │
         │found=NO? │  │
         └─────┬────┘  │
               │       │
          ┌────┴────┐  │
         YES       NO  │
          │         │  │
       Break        └──┘
          │
          ▼
    Return path
          │
          ▼
         END
```

**Output**: Chuỗi edges liền kề bị tắc

---

## 9. Bảng Tổng Hợp

### So Sánh Thuật Toán

| Thuật toán | Input | Output | Độ phức tạp |
|-----------|-------|--------|-------------|
| Dijkstra | start, goal | path, time | O((V+E) log V) |
| Alternative | blockedEdge | alt path | O((V+E) log V) |
| Congestion | - | congested list | O(E log E) |
| Bypass Gen | edge, budget | proposals | O(V(V+E) log V) |
| Adjacent | startEdge | path | O(E) |
| Best Select | proposals | best | O(n) |

### Mối Quan Hệ

```
        Dijkstra
            │
    ┌───────┼───────┐
    │       │       │
    ▼       ▼       ▼
  Alt   Bypass   Display
        │
        ▼
    Adjacent
        │
        ▼
   Optimization
        │
        ▼
     Select
```

---

## 10. Ví Dụ Thực Tế

### Kịch Bản 1: Tìm Đường Ngắn Nhất
```
Input: A → E

Dijkstra:
1. dist[A]=0, others=∞
2. Relax A→B(5), A→C(8)
3. Choose B: B→E(12)
4. Choose C: C→E(11) ← Tốt hơn!

Output: A → C → E (11 phút)
```

### Kịch Bản 2: Tối Ưu Giao Thông
```
Input:
- Edge E16: flow=2300, cap=2000
- Budget: 200 tỷ
- Cost: 150 tỷ

Phân tích:
- Quá tải 15%
- Budget ≥ cost → CẤP 1
- Tìm bypass qua F

Output:
- Xây E→F→G
- Chi phí: 175 tỷ
- Giảm 50% lưu lượng
```

---

## 11. Kết Luận

### Điểm Mạnh
1. **Modular**: Thuật toán độc lập
2. **Hiệu quả**: Cấu trúc dữ liệu tối ưu
3. **Thực tế**: Dựa trên dữ liệu thực
4. **Linh hoạt**: Dễ mở rộng

### Khuyến Nghị
- Phù hợp mạng quy mô vừa (V < 10,000)
- Cần cập nhật dữ liệu định kỳ
- Kết hợp giám sát real-time

---

**Tài liệu tham khảo**: ALGORITHM_REPORT.md  
**Ngày tạo**: 2025-12-16  
**Phiên bản**: 1.0
