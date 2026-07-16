#include <iostream>
#include <cassert>
#include <memory>
#include "../../include/model/Board.h"
#include "../../include/model/Position.h"
#include "../../include/model/Piece.h"
#include "../../include/model/Piece_enums.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void testInsideBounds() {
    Board board(8, 8);
    assert(board.isInsideBounds(Position(0, 0)) == true);
    assert(board.isInsideBounds(Position(7, 7)) == true);
    assert(board.isInsideBounds(Position(8, 4)) == false);
    assert(board.isInsideBounds(Position(3, -1)) == false);
    log_test("testInsideBounds");
}

void testAddPieceSuccess() {
    Board board(8, 8);
    auto piece = std::make_unique<Piece>(PieceColor::WHITE, PieceKind::PAWN);
    Piece* rawPiece = piece.get();

    assert(board.addPiece(Position(3, 3), std::move(piece)) == true);
    assert(board.getPieceAt(Position(3, 3)) == rawPiece);
    log_test("testAddPieceSuccess");
}

void testAddPieceOutsideBounds() {
    Board board(8, 8);
    auto piece = std::make_unique<Piece>(PieceColor::WHITE, PieceKind::PAWN);

    assert(board.addPiece(Position(10, 2), std::move(piece)) == false);
    assert(board.getPieceAt(Position(10, 2)) == nullptr);
    log_test("testAddPieceOutsideBounds");
}

void testRejectDuplicateOccupation() {
    Board board(8, 8);
    auto piece1 = std::make_unique<Piece>(PieceColor::WHITE, PieceKind::PAWN);
    auto piece2 = std::make_unique<Piece>(PieceColor::BLACK, PieceKind::PAWN);
    Piece* rawPiece1 = piece1.get();

    assert(board.addPiece(Position(4, 4), std::move(piece1)) == true);
    assert(board.addPiece(Position(4, 4), std::move(piece2)) == false);
    assert(board.getPieceAt(Position(4, 4)) == rawPiece1);
    log_test("testRejectDuplicateOccupation");
}

void testRemovePiece() {
    Board board(8, 8);
    auto piece = std::make_unique<Piece>(PieceColor::WHITE, PieceKind::PAWN);
    board.addPiece(Position(2, 2), std::move(piece));

    assert(board.removePiece(Position(2, 2)) == true);
    assert(board.getPieceAt(Position(2, 2)) == nullptr);
    assert(board.removePiece(Position(2, 2)) == false);
    log_test("testRemovePiece");
}

void testMovePieceToEmptySquare() {
    Board board(8, 8);
    auto piece = std::make_unique<Piece>(PieceColor::WHITE, PieceKind::PAWN);
    Piece* rawPiece = piece.get();
    board.addPiece(Position(1, 1), std::move(piece));

    board.movePiece(Position(1, 1), Position(1, 2));

    assert(board.getPieceAt(Position(1, 1)) == nullptr);
    assert(board.getPieceAt(Position(1, 2)) == rawPiece);
    assert(rawPiece->position == Position(1, 2));
    log_test("testMovePieceToEmptySquare");
}

void testMovePieceCapturesOccupant() {
    Board board(8, 8);
    auto mover = std::make_unique<Piece>(PieceColor::WHITE, PieceKind::ROOK);
    auto victim = std::make_unique<Piece>(PieceColor::BLACK, PieceKind::PAWN);
    Piece* rawMover = mover.get();

    board.addPiece(Position(0, 0), std::move(mover));
    board.addPiece(Position(0, 5), std::move(victim));

    board.movePiece(Position(0, 0), Position(0, 5));

    assert(board.getPieceAt(Position(0, 0)) == nullptr);
    assert(board.getPieceAt(Position(0, 5)) == rawMover);
    log_test("testMovePieceCapturesOccupant");
}

void testMovePieceFromEmptySourceIsNoOp() {
    Board board(8, 8);
    board.movePiece(Position(3, 3), Position(4, 4));
    assert(board.getPieceAt(Position(3, 3)) == nullptr);
    assert(board.getPieceAt(Position(4, 4)) == nullptr);
    log_test("testMovePieceFromEmptySourceIsNoOp");
}

void testGetAllPieces() {
    Board board(8, 8);
    board.addPiece(Position(0, 0), std::make_unique<Piece>(PieceColor::WHITE, PieceKind::ROOK));
    board.addPiece(Position(7, 7), std::make_unique<Piece>(PieceColor::BLACK, PieceKind::KING));

    auto all = board.getAllPieces();
    assert(all.size() == 2);
    log_test("testGetAllPieces");
}

int main() {
    testInsideBounds();
    testAddPieceSuccess();
    testAddPieceOutsideBounds();
    testRejectDuplicateOccupation();
    testRemovePiece();
    testMovePieceToEmptySquare();
    testMovePieceCapturesOccupant();
    testMovePieceFromEmptySourceIsNoOp();
    testGetAllPieces();
    std::cout << "All Board tests passed successfully!" << std::endl;
    return 0;
}
