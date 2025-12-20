#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>
#include "RoadMap.h"

// Colors
struct Color {
    Uint8 r, g, b, a;
    Color(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
};

// Button structure
struct Button {
    SDL_Rect rect;
    std::string label;
    int id;
    bool hovered;
    
    Button(int x, int y, int w, int h, const std::string& text, int buttonId)
        : rect{x, y, w, h}, label(text), id(buttonId), hovered(false) {}
    
    bool contains(int mouseX, int mouseY) const {
        return mouseX >= rect.x && mouseX < rect.x + rect.w &&
               mouseY >= rect.y && mouseY < rect.y + rect.h;
    }
};

class GuiRenderer {
public:
    GuiRenderer();
    ~GuiRenderer();
    
    bool init(const std::string& title, int width, int height);
    void cleanup();
    
    // Event handling
    bool pollEvent(SDL_Event& event);
    void handleMouseMotion(int x, int y);
    int handleMouseClick(int x, int y);
    
    // Drawing functions
    void clear(const Color& color);
    void present();
    
    // Note: fontSize parameter is currently not implemented - font size is set at initialization
    void drawText(const std::string& text, int x, int y, const Color& color, int fontSize = 20);
    void drawRect(int x, int y, int w, int h, const Color& color, bool filled = true);
    void drawLine(int x1, int y1, int x2, int y2, const Color& color, int thickness = 2);
    void drawCircle(int cx, int cy, int radius, const Color& color, bool filled = true);
    
    // UI Components
    void drawButton(const Button& button);
    void addButton(const Button& button);
    void clearButtons();
    
    // Map drawing
    void drawMap(RoadMap& map, int offsetX, int offsetY, double scale);
    void drawMapNode(const std::string& nodeId, double x, double y, const Color& color, int radius = 8);
    void drawMapEdge(double x1, double y1, double x2, double y2, const Color& color, int thickness = 2);
    void highlightPath(RoadMap& map, const std::vector<std::string>& path, int offsetX, int offsetY, double scale);
    std::string getEdgeIdBetweenNodes(RoadMap& map, const std::string& srcNode, const std::string& dstNode);
    
    // Helper functions
    void drawTitle(const std::string& title);
    void drawPanel(int x, int y, int w, int h, const std::string& title);
    void drawSmallText(const std::string& text, int x, int y, const Color& color);
    
    int getWidth() const { return windowWidth; }
    int getHeight() const { return windowHeight; }
    
    // Public access to buttons for external rendering
    std::vector<Button> buttons;
    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* titleFont;
    TTF_Font* smallFont;
    
    int windowWidth;
    int windowHeight;
    
    // Default map center coordinates (Vietnam region)
    static constexpr double DEFAULT_CENTER_LON = 107.5;
    static constexpr double DEFAULT_CENTER_LAT = 12.05;
    
    // Helper to convert lat/lon to screen coordinates
    void latLonToScreen(double lat, double lon, int& x, int& y, 
                       int offsetX, int offsetY, double scale);
};
