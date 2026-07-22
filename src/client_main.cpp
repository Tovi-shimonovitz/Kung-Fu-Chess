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
#include "graphics/window/win32_prompt_dialog.h"
#include "graphics/game_view_bindings.h"
#include "graphics/sprites/sprite_repository.h"
#include "input/board_mapper.h"
#include "input/controller.h"
#include "io/env_config.h"
#include "network/client/network_client.h"
#include "network/protocol/messages.h"

int main() {
    try {
        SetProcessDPIAware();

        PromptResult login = Win32PromptDialog("Kung-Fu Chess - Login", {"Username"}, {"OK"}).show();
        std::string username = login.values[0];

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

        networkClient.send(RegisterMessage(username));
        networkClient.send(PlayRequestMessage());
        std::cout << "Waiting for an opponent..." << std::endl;

        BoardMapper boardMapper;
        SpriteRepository spriteRepository(env.get("SPRITE_REPOSITORY_PATH"));
        BoardRenderer boardRenderer(env.get("BOARD_IMAGE_PATH"), spriteRepository);
        BoardFrameManager boardLayout(boardMapper, cellsWide, cellsHigh);

        Canvas canvas(env.getInt("CANVAS_WIDTH"), env.getInt("CANVAS_HEIGHT"), env.getInt("CANVAS_CHANNELS"));
        RenderableElement boardElement;
        bindBoardElement(canvas, boardRenderer, boardElement, boardLayout);

        Controller controller(boardMapper, [&networkClient](Position src, Position dst) {
            networkClient.send(MoveRequestMessage(src, dst));
        });
        GraphicsRunner graphicsRunner(canvas, controller, "KungFuChess - " + username);

        auto previousTime = std::chrono::steady_clock::now();
        while (!graphicsRunner.shouldQuit()) {
            auto frameStart = std::chrono::steady_clock::now();
            int elapsedMs = static_cast<int>(
                std::chrono::duration_cast<std::chrono::milliseconds>(frameStart - previousTime).count());
            previousTime = frameStart;

            networkClient.poll();

            if (auto snapshot = networkClient.latestSnapshot()) {
                canvas.refreshAll(*snapshot, elapsedMs);
                controller.updateSnapshot(*snapshot);
            }
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
