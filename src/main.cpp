#pragma once
#include <chrono>
#include <iostream>
#include <memory>
#include "engine/GameEngine.h"
#include "graphics/board_frame_manager.h"
#include "graphics/board_renderer.h"
#include "graphics/graphics_runner.h"
#include "graphics/sprite_repository.h"
#include "input/board_mapper.h"
#include "input/controller.h"
#include "io/BoardParser.h"
#include "model/Board.h"

int main() {
    try {
        GameEngine engine;
        engine.setBoard(std::make_unique<Board>(
            BoardParser::parseFromCsv("third_party/images/board.csv")));

        BoardMapper boardMapper;
        SpriteRepository spriteRepository("third_party/images");
        BoardRenderer renderer("third_party/images/board.png", spriteRepository);
        BoardFrameManager frameManager(boardMapper);
        Controller controller(engine, boardMapper);
        GraphicsRunner graphicsRunner(engine, renderer, frameManager, controller, "KungFuChess");

        std::cout << "Window open - press ESC or 'q' inside it to quit." << std::endl;

        auto lastTime = std::chrono::steady_clock::now();
        while (!graphicsRunner.shouldQuit()) {
            auto now = std::chrono::steady_clock::now();
            int elapsedMs = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count());
            lastTime = now;

            if (elapsedMs <= 0) {
                elapsedMs = 1;
            }

            engine.wait(elapsedMs);
        }

        std::cout << "Window closed." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
