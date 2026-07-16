#pragma once
#include <string>
#include "board_renderer.h"
#include "../engine/GameEngine.h"

class GraphicsRunner {
public:
    GraphicsRunner(GameEngine& engine, BoardRenderer& renderer, std::string windowName);

    bool shouldQuit() const { return quitRequested; }

private:
    void onWait(int elapsedMs);

    GameEngine& engine;
    BoardRenderer& renderer;
    std::string windowName;
    bool quitRequested;
};
