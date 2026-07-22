#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include "graphics/board/board_frame_manager.h"
#include "graphics/board/board_renderer.h"
#include "graphics/window/canvas.h"
#include "graphics/window/graphics_runner.h"
#include "graphics/window/renderable_element.h"
#include "graphics/game_view_bindings.h"
#include "graphics/sprites/sprite_repository.h"
#include "input/board_mapper.h"
#include "input/controller.h"
#include "io/env_config.h"
#include "network/client/network_client.h"
#include "network/protocol/messages.h"

// Same as client_main.cpp, minus the interactive Win32 login dialog
// (hardcoded username instead), so this can be driven non-interactively
// to reproduce the crash.
int main(int argc, char** argv) {
    try {
        std::string username = argc > 1 ? argv[1] : "diag";

        EnvConfig env = EnvConfig::load(".env");
        int targetFrameMs = env.getInt("TARGET_FRAME_MS");
        const int cellsWide = 8;
        const int cellsHigh = 8;

        NetworkClient networkClient;
        networkClient.connect("ws://localhost:9002");

        std::cout << "Connecting..." << std::endl;
        for (int i = 0; i < 40; ++i) {
            networkClient.poll();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        networkClient.send(RegisterMessage(username, "pw"));
        networkClient.send(PlayRequestMessage());
        std::cout << "Waiting for an opponent..." << std::endl;

        BoardMapper boardMapper;
        SpriteRepository spriteRepository(env.get("SPRITE_REPOSITORY_PATH"));
        BoardRenderer boardRenderer(env.get("BOARD_IMAGE_PATH"), spriteRepository);
        BoardFrameManager boardLayout(boardMapper, cellsWide, cellsHigh);

        Canvas canvas(env.getInt("CANVAS_WIDTH"), env.getInt("CANVAS_HEIGHT"), env.getInt("CANVAS_CHANNELS"));
        RenderableElement boardElement;
        bindBoardElement(canvas, boardRenderer, boardElement, boardLayout);
        std::cout << "Canvas/BoardRenderer wired up." << std::endl;

        Controller controller([&networkClient](Position src, Position dst) {
            networkClient.send(MoveRequestMessage(src, dst));
        });
        std::cout << "Creating GraphicsRunner (opens window)..." << std::endl;
        GraphicsRunner graphicsRunner(canvas, controller, boardMapper, "KungFuChess - " + username);
        std::cout << "GraphicsRunner created." << std::endl;

        GameSnapshot emptyBoardSnapshot{cellsWide, cellsHigh, {}, false};
        bool sawFirstRealSnapshot = false;

        int frameCount = 0;
        auto previousTime = std::chrono::steady_clock::now();
        while (!graphicsRunner.shouldQuit() && frameCount < 100) {
            auto frameStart = std::chrono::steady_clock::now();
            int elapsedMs = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(frameStart - previousTime).count());
            previousTime = frameStart;

            networkClient.poll();

            auto realSnapshot = networkClient.latestSnapshot();
            if (realSnapshot && !sawFirstRealSnapshot) {
                std::cout << "First real snapshot arrived, rendering..." << std::endl;
                sawFirstRealSnapshot = true;
            }
            GameSnapshot snapshot = realSnapshot.value_or(emptyBoardSnapshot);
            canvas.refreshAll(snapshot, elapsedMs);
            controller.updateSnapshot(snapshot);
            graphicsRunner.render();
            ++frameCount;

            auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - frameStart).count();
            if (frameDuration < targetFrameMs) {
                std::this_thread::sleep_for(std::chrono::milliseconds(targetFrameMs - frameDuration));
            }
        }
        std::cout << "Loop finished after " << frameCount << " frames without crashing." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "CRASHED: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
