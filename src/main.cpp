#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "engine/GameEngine.h"
#include "graphics/board/board_frame_manager.h"
#include "graphics/board/board_renderer.h"
#include "graphics/window/canvas.h"
#include "graphics/window/graphics_runner.h"
#include "graphics/window/renderable_element.h"
#include "graphics/game_view_bindings.h"
#include "graphics/sprites/sprite_repository.h"
#include "input/board_mapper.h"
#include "input/controller.h"
#include "io/BoardParser.h"
#include "io/env_config.h"
#include "model/Board.h"

int main() {
    try {
#ifdef _WIN32
        SetProcessDPIAware();
#endif

        EnvConfig env = EnvConfig::load(".env");
        int targetFrameMs = env.getInt("TARGET_FRAME_MS");

        Board board = BoardParser::parseFromCsv(env.get("BOARD_CSV_PATH"));
        std::cout << "board width " << board.width << std::endl;
        int cellsWide = board.width;
        int cellsHigh = board.height;

        GameEngine engine;
        engine.setBoard(std::make_unique<Board>(std::move(board)));

        BoardMapper boardMapper;
        SpriteRepository spriteRepository(env.get("SPRITE_REPOSITORY_PATH"));
        BoardRenderer boardRenderer(env.get("BOARD_IMAGE_PATH"), spriteRepository);
        BoardFrameManager boardLayout(boardMapper, cellsWide, cellsHigh);

        Canvas canvas(env.getInt("CANVAS_WIDTH"), env.getInt("CANVAS_HEIGHT"), env.getInt("CANVAS_CHANNELS"));
        RenderableElement boardElement;
        bindBoardElement(canvas, boardRenderer, boardElement, boardLayout);

        Controller controller(engine, boardMapper);
        GraphicsRunner graphicsRunner(canvas, controller, "KungFuChess");

        auto previousTime = std::chrono::steady_clock::now();
        while (!graphicsRunner.shouldQuit()) {
            auto frameStart = std::chrono::steady_clock::now();
            int elapsedMs = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(frameStart - previousTime).count());
            previousTime = frameStart;

            engine.wait(elapsedMs);
            canvas.refreshAll(engine.getSnapshot(), elapsedMs);
            graphicsRunner.render();

            auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - frameStart).count();
            if (frameDuration < targetFrameMs) {
                std::this_thread::sleep_for(std::chrono::milliseconds(targetFrameMs - frameDuration));
            }
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
