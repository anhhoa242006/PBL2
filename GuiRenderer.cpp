#include "GuiRenderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>

GuiRenderer::GuiRenderer()
    : window(nullptr), renderer(nullptr), font(nullptr), titleFont(nullptr), smallFont(nullptr),
      windowWidth(0), windowHeight(0) {
}

GuiRenderer::~GuiRenderer() {
    cleanup();
}

bool GuiRenderer::init(const std::string& title, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        return false;
    }
    
    // Create window
    window = SDL_CreateWindow(title.c_str(),
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             width, height,
                             SDL_WINDOW_SHOWN);
    if (!window) {
        return false;
    }
    
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        return false;
    }
    
    // Load fonts - try to use a default system font, fall back to SDL's built-in if needed
    font = TTF_OpenFont("assets/fonts/arial.ttf", 18);
    if (!font) {
        // Try alternative font path
        font = TTF_OpenFont("assets/fonts/arial.ttf", 18);
    }
    
    titleFont = TTF_OpenFont("assets/fonts/arial.ttf", 24);
    if (!titleFont && font) {
        titleFont = font; // Use regular font if bold is not available
    }
    
    smallFont = TTF_OpenFont("assets/fonts/arial.ttf", 12);
    if (!smallFont && font) {
        smallFont = font; // Use regular font if small is not available
    }
    
    return true;
}

void GuiRenderer::cleanup() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (titleFont && titleFont != font) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (smallFont && smallFont != font && smallFont != titleFont) {
        TTF_CloseFont(smallFont);
        smallFont = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}

bool GuiRenderer::pollEvent(SDL_Event& event) {
    return SDL_PollEvent(&event) != 0;
}

void GuiRenderer::handleMouseMotion(int x, int y) {
    for (auto& button : buttons) {
        button.hovered = button.contains(x, y);
    }
}

int GuiRenderer::handleMouseClick(int x, int y) {
    for (const auto& button : buttons) {
        if (button.contains(x, y)) {
            return button.id;
        }
    }
    return -1;
}

void GuiRenderer::clear(const Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void GuiRenderer::present() {
    SDL_RenderPresent(renderer);
}

void GuiRenderer::drawText(const std::string& text, int x, int y, const Color& color, int fontSize) {
    (void)fontSize;
    if (! font) return;
    
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), sdlColor);
    if (!surface) {
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void GuiRenderer::drawSmallText(const std::string& text, int x, int y, const Color& color) {
    TTF_Font* fontToUse = smallFont ? smallFont : font;
    if (!fontToUse) return;
    
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(fontToUse, text.c_str(), sdlColor);
    if (!surface) {
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}


void GuiRenderer::drawRect(int x, int y, int w, int h, const Color& color, bool filled) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (filled) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void GuiRenderer::drawLine(int x1, int y1, int x2, int y2, const Color& color, int thickness) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    if (thickness <= 1) {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    } else {
        // Draw thick line by drawing multiple lines
        double angle = std::atan2(y2 - y1, x2 - x1);
        double perpAngle = angle + M_PI / 2.0;
        
        for (int i = -(thickness/2); i <= thickness/2; i++) {
            int offsetX = static_cast<int>(i * std::cos(perpAngle));
            int offsetY = static_cast<int>(i * std::sin(perpAngle));
            SDL_RenderDrawLine(renderer, x1 + offsetX, y1 + offsetY, 
                             x2 + offsetX, y2 + offsetY);
        }
    }
}

void GuiRenderer::drawCircle(int cx, int cy, int radius, const Color& color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                if (filled || std::abs(std::sqrt(dx*dx + dy*dy) - radius) < 1.5) {
                    SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                }
            }
        }
    }
}

void GuiRenderer::drawButton(const Button& button) {
    // Draw button background
    Color bgColor = button.hovered ? Color(70, 130, 180) : Color(50, 100, 150);
    drawRect(button.rect.x, button.rect.y, button.rect.w, button.rect.h, bgColor, true);
    
    // Draw button border
    drawRect(button.rect.x, button.rect.y, button.rect.w, button.rect.h, Color(200, 200, 200), false);
    
    // Draw button text (centered)
    if (font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, button.label.c_str(), white);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                int textX = button.rect.x + (button.rect.w - surface->w) / 2;
                int textY = button.rect.y + (button.rect.h - surface->h) / 2;
                SDL_Rect textRect = {textX, textY, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

void GuiRenderer::addButton(const Button& button) {
    buttons.push_back(button);
}

void GuiRenderer::clearButtons() {
    buttons.clear();
}

void GuiRenderer::latLonToScreen(double lat, double lon, int& x, int& y, 
                                 int offsetX, int offsetY, double scale) {
    x = static_cast<int>((lon - DEFAULT_CENTER_LON) * scale * 1000) + offsetX;
    y = static_cast<int>((DEFAULT_CENTER_LAT - lat) * scale * 1000) + offsetY;
}

void GuiRenderer::drawMap(RoadMap& map, int offsetX, int offsetY, double scale) {
    auto nodeIds = map.getNodeIds();
    auto edges = map.getEdges();
    
    if (nodeIds.empty()) {
        drawText("Bản đồ trống", offsetX + 150, offsetY + 100, Color(150, 150, 150));
        return;
    }
    
    // Tính toán bounding box tự động
    double minLat = 1e9, maxLat = -1e9, minLon = 1e9, maxLon = -1e9;
    for (const auto& nodeId : nodeIds) {
        auto node = map.getNodeById(nodeId);
        if (node) {
            minLat = std::min(minLat, node->lat);
            maxLat = std::max(maxLat, node->lat);
            minLon = std::min(minLon, node->lon);
            maxLon = std::max(maxLon, node->lon);
        }
    }
    
    // Tính scale tự động để vừa với panel (420x380 pixel)
    double latRange = maxLat - minLat;
    double lonRange = maxLon - minLon;
    double autoScale = std::min(380.0 / (latRange * 1000), 420.0 / (lonRange * 1000)) * 0.9;
    double centerLat = (minLat + maxLat) / 2.0;
    double centerLon = (minLon + maxLon) / 2.0;
    
    // Vẽ edges với màu dựa trên traffic load
    for (const auto& edge :  edges) {
        if (! edge.isReverse) {
            auto srcNode = map.getNodeById(edge.src);
            auto dstNode = map.getNodeById(edge.dst);
            
            if (srcNode && dstNode) {
                int x1 = static_cast<int>((srcNode->lon - centerLon) * autoScale * 1000) + offsetX + 210;
                int y1 = static_cast<int>((centerLat - srcNode->lat) * autoScale * 1000) + offsetY + 190;
                int x2 = static_cast<int>((dstNode->lon - centerLon) * autoScale * 1000) + offsetX + 210;
                int y2 = static_cast<int>((centerLat - dstNode->lat) * autoScale * 1000) + offsetY + 190;
                
                // Color based on traffic load - more clear distinction
                Color edgeColor(100, 100, 100);
                if (edge.flow > edge.capacity * 0.9) {
                    edgeColor = Color(220, 20, 20);  // Red for heavy congestion
                } else if (edge.flow > edge.capacity * 0.7) {
                    edgeColor = Color(220, 160, 20);  // Orange for moderate congestion
                } else {
                    edgeColor = Color(60, 180, 60);  // Green for free flow
                }
                
                drawLine(x1, y1, x2, y2, edgeColor, 3);
                
                // Draw edge ID at the midpoint with small text
                int midX = (x1 + x2) / 2;
                int midY = (y1 + y2) / 2;
                
                // Draw background for edge ID
                drawRect(midX - 15, midY - 8, 30, 16, Color(30, 30, 40, 220), true);
                drawSmallText(edge.id, midX - 12, midY - 6, Color(255, 255, 150));
            }
        }
    }
    
    // Vẽ nodes with IDs displayed
    for (const auto& nodeId : nodeIds) {
        auto node = map.getNodeById(nodeId);
        if (node) {
            int x = static_cast<int>((node->lon - centerLon) * autoScale * 1000) + offsetX + 210;
            int y = static_cast<int>((centerLat - node->lat) * autoScale * 1000) + offsetY + 190;
            
            drawCircle(x, y, 5, Color(70, 130, 180), true);
            drawCircle(x, y, 5, Color(255, 255, 255), false);
            
            // Draw node ID next to the node
            drawSmallText(nodeId, x + 8, y - 6, Color(200, 220, 255));
        }
    }
}

void GuiRenderer::drawMapNode(const std::string& nodeId, double x, double y, 
                              const Color& color, int radius) {
    drawCircle(static_cast<int>(x), static_cast<int>(y), radius, color, true);
    drawCircle(static_cast<int>(x), static_cast<int>(y), radius, Color(255, 255, 255), false);
    drawText(nodeId, static_cast<int>(x) + radius + 5, static_cast<int>(y) - 5, Color(255, 255, 255));
}

void GuiRenderer::drawMapEdge(double x1, double y1, double x2, double y2, 
                              const Color& color, int thickness) {
    drawLine(static_cast<int>(x1), static_cast<int>(y1), 
             static_cast<int>(x2), static_cast<int>(y2), color, thickness);
}

void GuiRenderer::highlightPath(RoadMap& map, const std::vector<std::string>& path, 
                                int offsetX, int offsetY, double scale) {
    (void)scale;  // Unused parameter - we calculate our own scale
    
    if (path.size() < 2) return;
    
    auto nodeIds = map.getNodeIds();
    if (nodeIds.empty()) return;
    
    // Calculate bounding box and auto-scale (same as drawMap)
    double minLat = 1e9, maxLat = -1e9, minLon = 1e9, maxLon = -1e9;
    for (const auto& nodeId : nodeIds) {
        auto node = map.getNodeById(nodeId);
        if (node) {
            minLat = std::min(minLat, node->lat);
            maxLat = std::max(maxLat, node->lat);
            minLon = std::min(minLon, node->lon);
            maxLon = std::max(maxLon, node->lon);
        }
    }
    
    double latRange = maxLat - minLat;
    double lonRange = maxLon - minLon;
    double autoScale = std::min(380.0 / (latRange * 1000), 420.0 / (lonRange * 1000)) * 0.9;
    double centerLat = (minLat + maxLat) / 2.0;
    double centerLon = (minLon + maxLon) / 2.0;
    
    // Draw highlighted path with edge IDs
    for (size_t i = 0; i < path.size() - 1; i++) {
        auto srcNode = map.getNodeById(path[i]);
        auto dstNode = map.getNodeById(path[i+1]);
        
        if (srcNode && dstNode) {
            int x1 = static_cast<int>((srcNode->lon - centerLon) * autoScale * 1000) + offsetX + 210;
            int y1 = static_cast<int>((centerLat - srcNode->lat) * autoScale * 1000) + offsetY + 190;
            int x2 = static_cast<int>((dstNode->lon - centerLon) * autoScale * 1000) + offsetX + 210;
            int y2 = static_cast<int>((centerLat - dstNode->lat) * autoScale * 1000) + offsetY + 190;
            
            // Draw thick highlighted line
            drawLine(x1, y1, x2, y2, Color(255, 200, 0), 6);  // Thicker yellow highlight
            
            // Get and display edge ID
            std::string edgeId = getEdgeIdBetweenNodes(map, path[i], path[i+1]);
            if (!edgeId.empty()) {
                int midX = (x1 + x2) / 2;
                int midY = (y1 + y2) / 2;
                
                // Draw background for edge ID text
                drawRect(midX - 20, midY - 10, 40, 20, Color(50, 50, 50, 200), true);
                drawText(edgeId, midX - 15, midY - 8, Color(255, 255, 100), 16);
            }
        }
    }
    
    // Draw path nodes with larger highlight
    for (const auto& nodeId : path) {
        auto node = map.getNodeById(nodeId);
        if (node) {
            int x = static_cast<int>((node->lon - centerLon) * autoScale * 1000) + offsetX + 210;
            int y = static_cast<int>((centerLat - node->lat) * autoScale * 1000) + offsetY + 190;
            
            drawCircle(x, y, 10, Color(255, 200, 0), true);
            drawCircle(x, y, 10, Color(255, 255, 255), false);
            drawText(nodeId, x + 12, y - 5, Color(255, 255, 100));
        }
    }
}

std::string GuiRenderer::getEdgeIdBetweenNodes(RoadMap& map, const std::string& srcNode, const std::string& dstNode) {
    auto edges = map.outgoingEdges(srcNode);
    for (const auto& edge : edges) {
        if (edge.dst == dstNode) {
            return edge.id;
        }
    }
    return "";
}

void GuiRenderer::drawTitle(const std::string& title) {
    if (!titleFont) return;
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Blended(titleFont, title.c_str(), white);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    int x = (windowWidth - surface->w) / 2;
    int y = 20;
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void GuiRenderer::drawPanel(int x, int y, int w, int h, const std::string& title) {
    // Draw panel background
    drawRect(x, y, w, h, Color(30, 30, 40, 200), true);
    
    // Draw panel border
    drawRect(x, y, w, h, Color(100, 100, 120), false);
    
    // Draw panel title
    if (!title.empty()) {
        drawRect(x, y, w, 30, Color(50, 50, 70), true);
        drawText(title, x + 10, y + 5, Color(255, 255, 255));
    }
}
