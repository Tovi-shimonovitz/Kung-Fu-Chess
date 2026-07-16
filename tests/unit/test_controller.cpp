#include <cassert>
#include <iostream>
#include <memory>
#include "../../include/input/controller.h"
#include "../../include/engine/GameEngine.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

int pixelFor(int cell) {
    return cell * CELL_SIZE + CELL_SIZE / 2;
}

void test_click_selects_piece_then_moves_to_destination() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({1, 2}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {1, 2}));
    engine.setBoard(std::move(board));
    BoardMapper boardMapper;
    Controller controller(engine, boardMapper);

    controller.handleInput(pixelFor(2), pixelFor(1)); // select (1,2)
    controller.handleInput(pixelFor(5), pixelFor(1)); // request move to (1,5)

    engine.wait(3000); // 3 squares of travel time

    assert(engine.hasPieceAt({1, 2}) == false);
    assert(engine.hasPieceAt({1, 5}) == true);
    log_test("test_click_selects_piece_then_moves_to_destination");
}

void test_click_on_empty_square_selects_nothing() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({4, 4}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {4, 4}));
    engine.setBoard(std::move(board));
    BoardMapper boardMapper;
    Controller controller(engine, boardMapper);

    controller.handleInput(pixelFor(0), pixelFor(0)); // empty square: no selection made

    // A subsequent click on an actual piece should now select it fresh
    // (proving the earlier click on empty space left no stale selection).
    controller.handleInput(pixelFor(4), pixelFor(4));
    controller.handleInput(pixelFor(4), pixelFor(6));

    engine.wait(2000);

    assert(engine.hasPieceAt({4, 4}) == false);
    assert(engine.hasPieceAt({6, 4}) == true);
    log_test("test_click_on_empty_square_selects_nothing");
}

void test_click_outside_board_deselects_current_piece() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({2, 2}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {2, 2}));
    board->addPiece({5, 5}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::BISHOP, {5, 5}));
    engine.setBoard(std::move(board));
    BoardMapper boardMapper;
    Controller controller(engine, boardMapper);

    controller.handleInput(pixelFor(2), pixelFor(2));  // select rook at (2,2)
    controller.handleInput(-100, -100);                // click outside the board: deselect

    // This click should now select the bishop fresh, not move the rook.
    controller.handleInput(pixelFor(5), pixelFor(5));
    controller.handleInput(pixelFor(7), pixelFor(7));

    engine.wait(2000);

    assert(engine.hasPieceAt({2, 2}) == true); // rook never moved
    assert(engine.hasPieceAt({5, 5}) == false);
    assert(engine.hasPieceAt({7, 7}) == true);
    log_test("test_click_outside_board_deselects_current_piece");
}

void test_illegal_destination_does_not_move_piece() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    engine.setBoard(std::move(board));
    BoardMapper boardMapper;
    Controller controller(engine, boardMapper);

    controller.handleInput(pixelFor(0), pixelFor(0)); // select rook
    controller.handleInput(pixelFor(5), pixelFor(5)); // diagonal: illegal for a rook

    engine.wait(1000);

    assert(engine.hasPieceAt({0, 0}) == true);
    assert(engine.hasPieceAt({5, 5}) == false);
    log_test("test_illegal_destination_does_not_move_piece");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING CONTROLLER UNIT TESTS        " << std::endl;
    std::cout << "========================================" << std::endl;

    test_click_selects_piece_then_moves_to_destination();
    test_click_on_empty_square_selects_nothing();
    test_click_outside_board_deselects_current_piece();
    test_illegal_destination_does_not_move_piece();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL CONTROLLER TESTS EXECUTED         " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
