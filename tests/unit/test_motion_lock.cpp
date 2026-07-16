#include <iostream>
#include <cassert>
#include <memory>
#include "../../include/engine/GameEngine.h"
#include "../../include/engine/Result_structs.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"

namespace {

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

std::unique_ptr<Board> makeBoardWithRookAndKnight() {
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    board->addPiece({7, 1}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KNIGHT, {7, 1}));
    return board;
}

} 

void test_reject_second_move_on_same_piece_while_moving() {
    GameEngine engine;
    engine.setBoard(makeBoardWithRookAndKnight());

    auto first = engine.requestMove({0, 0}, {0, 4});
    assert(first.is_accepted == true);
    assert(first.reason == "move_started");

    auto second = engine.requestMove({0, 0}, {0, 2});
    assert(second.is_accepted == false);
    assert(second.reason == "motion_in_progress");

    log_test("test_reject_second_move_on_same_piece_while_moving");
}

void test_reject_move_regardless_of_requested_destination() {
    GameEngine engine;
    engine.setBoard(makeBoardWithRookAndKnight());

    auto first = engine.requestMove({0, 0}, {3, 0});
    assert(first.is_accepted == true);

    // Same source, a different (still otherwise legal) destination than the first call.
    auto second = engine.requestMove({0, 0}, {0, 5});
    assert(second.is_accepted == false);
    assert(second.reason == "motion_in_progress");

    log_test("test_reject_move_regardless_of_requested_destination");
}

void test_other_piece_can_move_while_first_is_moving() {
    GameEngine engine;
    engine.setBoard(makeBoardWithRookAndKnight());

    auto rookMove = engine.requestMove({0, 0}, {0, 4});
    assert(rookMove.is_accepted == true);


    auto knightMove = engine.requestMove({7, 1}, {5, 0});
    assert(knightMove.is_accepted == true);
    assert(knightMove.reason == "move_started");

    log_test("test_other_piece_can_move_while_first_is_moving");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING MOTION LOCK UNIT TESTS        " << std::endl;
    std::cout << "========================================" << std::endl;

    test_reject_second_move_on_same_piece_while_moving();
    test_reject_move_regardless_of_requested_destination();
    test_other_piece_can_move_while_first_is_moving();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL MOTION LOCK TESTS EXECUTED         " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
