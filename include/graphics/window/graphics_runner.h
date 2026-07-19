#pragma once
#include <cstddef>
#include <string>

class GameEngine;
class Canvas;
class Controller;

class GraphicsRunner {
public:
    GraphicsRunner(GameEngine& engine, Canvas& canvas, Controller& controller, std::string windowName);
    ~GraphicsRunner();

    bool shouldQuit() const { return quitRequested; }

private:
    static void mouseTrampoline(int event, int x, int y, int flags, void* userdata);
    void onMouse(int event, int x, int y);
    void onWait(int elapsedMs);

    GameEngine& engine;
    Canvas& canvas;
    Controller& controller;
    std::string windowName;
    bool quitRequested;
    std::size_t waitObserverId;
};
