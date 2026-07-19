#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../include/graphics/board/board_renderer.h"
#include "../../include/graphics/sprites/sprite_repository.h"
#include "../../include/engine/GameEngine.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"

int main() {
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({0, 4}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::KING, {0, 4}));
    board->addPiece({0, 3}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::QUEEN, {0, 3}));
    board->addPiece({1, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::PAWN, {1, 0}));
    board->addPiece({7, 4}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KING, {7, 4}));
    board->addPiece({7, 1}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KNIGHT, {7, 1}));

    GameEngine engine;
    engine.setBoard(std::move(board));

    SpriteRepository spriteRepository("third_party/images");
    BoardRenderer renderer("third_party/images/board.png", spriteRepository);

    Img frame = renderer.render(engine.getSnapshot(), 0);
    cv::imwrite("smoke_board_renderer_output.png", frame.get_mat());
    frame.show();

    std::cout << "Wrote smoke_board_renderer_output.png" << std::endl;
    return 0;
}
