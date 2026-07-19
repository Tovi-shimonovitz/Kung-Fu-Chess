#include <chrono>
#include <iostream>
#include <thread>
#include "../../include/graphics/window/graphics_runner.h"
#include "../../include/graphics/window/canvas.h"
#include "../../include/graphics/window/renderable_element.h"
#include "../../include/graphics/game_view_bindings.h"
#include "../../include/graphics/board/board_renderer.h"
#include "../../include/graphics/board/board_frame_manager.h"
#include "../../include/graphics/sprites/sprite_repository.h"
#include "../../include/input/controller.h"
#include "../../include/input/board_mapper.h"
#include "../../include/engine/GameEngine.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"

namespace {
int pixelForCell(int cell) {
    return cell * CELL_SIZE + CELL_SIZE / 2;
}
}

int main() {
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({0, 4}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::KING, {0, 4}));
    board->addPiece({0, 3}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::QUEEN, {0, 3}));
    board->addPiece({7, 4}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KING, {7, 4}));
    board->addPiece({7, 1}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KNIGHT, {7, 1}));

    GameEngine engine;
    engine.setBoard(std::move(board));

    SpriteRepository spriteRepository("third_party/images");
    BoardRenderer renderer("third_party/images/board.png", spriteRepository);
    BoardMapper boardMapper;
    BoardFrameManager frameManager(boardMapper, 8, 8);

    Canvas canvas(800, 800, 4);
    RenderableElement boardElement;
    canvas.registerElement(boardElement, frameManager);
    bindBoardElement(engine, renderer, boardElement);

    Controller controller(engine, boardMapper);
    GraphicsRunner graphicsRunner(engine, canvas, controller, "KungFuChess");

    // click the white queen at (0,3), then click destination (3,3): straight move down the column
    controller.handleInput(pixelForCell(3), pixelForCell(0));
    controller.handleInput(pixelForCell(3), pixelForCell(3));

    // click the black knight at (7,1), then click destination (5,2): an L-shaped knight move
    controller.handleInput(pixelForCell(1), pixelForCell(7));
    controller.handleInput(pixelForCell(2), pixelForCell(5));

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
    return 0;
}
