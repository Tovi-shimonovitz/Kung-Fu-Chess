#include <iostream>
#include <cassert>
#include <memory>
#include "../../include/engine/GameEngine.h"
#include "../../include/engine/Result_structs.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

std::unique_ptr<Board> makeEmptyBoard() {
    return std::make_unique<Board>(8, 8);
}

std::unique_ptr<Board> makeBoardWithRookAndKing() {
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    board->addPiece({0, 1}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KING, {0, 1}));
    return board;
}

void test_reject_no_piece_at_source() {
    GameEngine engine;
    engine.setBoard(makeEmptyBoard());

    auto result = engine.requestMove({7, 7}, {7, 6});
    assert(result.is_accepted == false);
    assert(result.reason == "no_piece_at_source");
    log_test("test_reject_no_piece_at_source");
}

void test_reject_illegal_move() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    engine.setBoard(std::move(board));

    auto result = engine.requestMove({0, 0}, {5, 5});
    assert(result.is_accepted == false);
    assert(result.reason == "illegal_piece_move");
    log_test("test_reject_illegal_move");
}

void test_accept_valid_move_and_arrival_updates_board() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({1, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {1, 0}));
    engine.setBoard(std::move(board));

    auto result = engine.requestMove({1, 0}, {1, 2});
    assert(result.is_accepted == true);
    assert(result.reason == "move_started");

    // Board bookkeeping only updates on arrival; mid-flight the piece is still at its source.
    assert(engine.hasPieceAt({1, 0}) == true);
    assert(engine.hasPieceAt({1, 2}) == false);

    engine.wait(2000); // 2 squares of distance -> 2000ms travel time

    assert(engine.hasPieceAt({1, 0}) == false);
    assert(engine.hasPieceAt({1, 2}) == true);
    log_test("test_accept_valid_move_and_arrival_updates_board");
}

void test_snapshot_consistency() {
    GameEngine engine;
    engine.setBoard(makeEmptyBoard());

    GameSnapshot s1 = engine.getSnapshot();
    assert(s1.isGameOver == false);
    assert(s1.width == 8);
    assert(s1.height == 8);
    assert(s1.pieces.empty());
    log_test("test_snapshot_consistency");
}

void test_snapshot_reflects_added_pieces() {
    GameEngine engine;
    auto board = std::make_unique<Board>(8, 8);
    board->addPiece({2, 2}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::QUEEN, {2, 2}));
    engine.setBoard(std::move(board));

    GameSnapshot snapshot = engine.getSnapshot();
    assert(snapshot.pieces.size() == 1);
    log_test("test_snapshot_reflects_added_pieces");
}

void test_king_capture_ends_game() {
    GameEngine engine;
    engine.setBoard(makeBoardWithRookAndKing());

    auto move = engine.requestMove({0, 0}, {0, 1});
    assert(move.is_accepted == true);

    WaitResult wr = engine.wait(1000); // 1 square of distance -> 1000ms
    assert(wr.message.find("king_eaten") != std::string::npos);
    assert(engine.getSnapshot().isGameOver == true);
    log_test("test_king_capture_ends_game");
}

void test_reject_move_after_game_over() {
    GameEngine engine;
    engine.setBoard(makeBoardWithRookAndKing());

    engine.requestMove({0, 0}, {0, 1});
    engine.wait(1000);

    auto result = engine.requestMove({0, 1}, {0, 2});
    assert(result.is_accepted == false);
    assert(result.reason == "game_over");
    log_test("test_reject_move_after_game_over");
}

void test_wait_after_game_over_is_rejected() {
    GameEngine engine;
    engine.setBoard(makeBoardWithRookAndKing());

    engine.requestMove({0, 0}, {0, 1});
    engine.wait(1000);

    WaitResult wr = engine.wait(100);
    assert(wr.was_processed == false);
    assert(wr.message == "game_over");
    log_test("test_wait_after_game_over_is_rejected");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING GAME ENGINE UNIT TESTS       " << std::endl;
    std::cout << "========================================" << std::endl;

    test_reject_no_piece_at_source();
    test_reject_illegal_move();
    test_accept_valid_move_and_arrival_updates_board();
    test_snapshot_consistency();
    test_snapshot_reflects_added_pieces();
    test_king_capture_ends_game();
    test_reject_move_after_game_over();
    test_wait_after_game_over_is_rejected();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL GAME ENGINE TESTS EXECUTED        " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
