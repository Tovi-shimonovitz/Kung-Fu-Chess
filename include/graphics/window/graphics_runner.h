#pragma once
#include <string>

class Canvas;
class Controller;
class BoardMapper;

class GraphicsRunner {
public:
    GraphicsRunner(Canvas& canvas, Controller& controller, BoardMapper& boardMapper, std::string windowName);

    bool shouldQuit() const { return quitRequested; }
    void render();

private:
    static void mouseTrampoline(int event, int x, int y, int flags, void* userdata);
    void onMouse(int event, int x, int y);

    Canvas& canvas;
    Controller& controller;
    BoardMapper& boardMapper;
    std::string windowName;
    bool quitRequested;
};
