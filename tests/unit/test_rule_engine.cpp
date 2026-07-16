#include <cassert>
#include <iostream>
#include <memory>
#include "../../include/model/Board.h"
#include "../../include/model/Position.h"
#include "../../include/model/Piece.h"
#include "../../include/rules/RuleEngine.h"

Piece* placePiece(Board& board, PieceColor color, PieceKind kind, Position pos) {
    auto piece = std::make_unique<Piece>(color, kind);
    piece->setPosition(pos);
    Piece* raw = piece.get();
    board.addPiece(pos, std::move(piece));
    return raw;
}

void test_validate_move_outside_board() {
    Board board(8, 8);
    RuleEngine engine;

    Position source(0, 0);
    Position target(-1, 0);

    auto result = engine.validateMove(board, source, target);

    assert(result.is_valid == false);
    assert(result.reason == MoveStatus::OUTSIDE_BOARD);
    assert(result.getReason() == "outside_board");
    std::cout << "test_validate_move_outside_board passed" << std::endl;
}

void test_validate_move_empty_source() {
    Board board(8, 8);
    RuleEngine engine;

    Position source(0, 0);
    Position target(0, 1);

    auto result = engine.validateMove(board, source, target);

    assert(result.is_valid == false);
    assert(result.reason == MoveStatus::EMPTY_SOURCE);
    assert(result.getReason() == "empty_source");
    std::cout << "test_validate_move_empty_source passed" << std::endl;
}

void test_validate_move_friendly_destination() {
    Board board(8, 8);
    RuleEngine engine;

    placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(0, 0));
    placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(0, 1));

    auto result = engine.validateMove(board, Position(0, 0), Position(0, 1));

    assert(result.is_valid == false);
    assert(result.reason == MoveStatus::FRIENDLY_DESTINATION);
    assert(result.getReason() == "friendly_destination");
    std::cout << "test_validate_move_friendly_destination passed" << std::endl;
}

void test_validate_move_illegal_for_piece() {
    Board board(8, 8);
    RuleEngine engine;

    placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(3, 3));

    auto result = engine.validateMove(board, Position(3, 3), Position(5, 5));

    assert(result.is_valid == false);
    assert(result.reason == MoveStatus::ILLEGAL_PIECE_MOVE);
    assert(result.getReason() == "illegal_piece_move");
    std::cout << "test_validate_move_illegal_for_piece passed" << std::endl;
}

void test_validate_move_valid_move() {
    Board board(8, 8);
    RuleEngine engine;

    placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(3, 3));

    auto result = engine.validateMove(board, Position(3, 3), Position(3, 5));

    assert(result.is_valid == true);
    assert(result.reason == MoveStatus::OK);
    assert(result.getReason() == "ok");
    std::cout << "test_validate_move_valid_move passed" << std::endl;
}

void test_validate_move_valid_capture_of_enemy() {
    Board board(8, 8);
    RuleEngine engine;

    placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(3, 3));
    placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(3, 6));

    auto result = engine.validateMove(board, Position(3, 3), Position(3, 6));

    assert(result.is_valid == true);
    assert(result.reason == MoveStatus::OK);
    std::cout << "test_validate_move_valid_capture_of_enemy passed" << std::endl;
}

void test_validate_move_target_outside_board() {
    Board board(8, 8);
    RuleEngine engine;

    placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(0, 0));

    auto result = engine.validateMove(board, Position(0, 0), Position(0, 8));

    assert(result.is_valid == false);
    assert(result.reason == MoveStatus::OUTSIDE_BOARD);
    std::cout << "test_validate_move_target_outside_board passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING RULE ENGINE UNIT TESTS       " << std::endl;
    std::cout << "========================================" << std::endl;

    test_validate_move_outside_board();
    test_validate_move_empty_source();
    test_validate_move_friendly_destination();
    test_validate_move_illegal_for_piece();
    test_validate_move_valid_move();
    test_validate_move_valid_capture_of_enemy();
    test_validate_move_target_outside_board();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL RULE ENGINE TESTS EXECUTED        " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
