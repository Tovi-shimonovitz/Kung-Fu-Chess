#include <cassert>
#include <iostream>
#include <memory>
#include <set>
#include "../../include/model/Board.h"
#include "../../include/model/Position.h"
#include "../../include/model/Piece.h"
#include "../../include/rules/piece_rules.h"

Piece* placePiece(Board& board, PieceColor color, PieceKind kind, Position pos) {
    auto piece = std::make_unique<Piece>(color, kind);
    piece->setPosition(pos);
    Piece* raw = piece.get();
    board.addPiece(pos, std::move(piece));
    return raw;
}

//Rook move along empty rows and empty columns
void testRookEmptyBoard() {
    Board board(8, 8);
    Piece* rook = placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(3, 3));

    RookRule rookRule;
    std::set<Position> dests = rookRule.legal_destinations(board, *rook);

    assert(dests.size() == 14);

    assert(dests.count(Position(3, 0)) == 1);
    assert(dests.count(Position(3, 7)) == 1);
    assert(dests.count(Position(0, 3)) == 1);
    assert(dests.count(Position(7, 3)) == 1);

    assert(dests.count(Position(3, 3)) == 0);

    std::cout << "testRookEmptyBoard passed!" << std::endl;
}

// Rook stopped before friendly piece
void testRookFriendlyBlocker() {
    Board board(8, 8);
    Piece* rook = placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(3, 3));
    placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(3, 5));

    RookRule rookRule;
    std::set<Position> dests = rookRule.legal_destinations(board, *rook);

    assert(dests.count(Position(3, 4)) == 1);
    assert(dests.count(Position(3, 5)) == 0);
    assert(dests.count(Position(3, 6)) == 0);

    std::cout << "testRookFriendlyBlocker passed!" << std::endl;
}

// Rook captures an enemy blocker but does not pass it
void testRookEnemyBlocker() {
    Board board(8, 8);
    Piece* rook = placePiece(board, PieceColor::WHITE, PieceKind::ROOK, Position(3, 3));
    placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(5, 3));

    RookRule rookRule;
    std::set<Position> dests = rookRule.legal_destinations(board, *rook);

    assert(dests.count(Position(4, 3)) == 1);
    assert(dests.count(Position(5, 3)) == 1);
    assert(dests.count(Position(6, 3)) == 0);

    std::cout << "testRookEnemyBlocker passed!" << std::endl;
}

//Bishop move along diagonals
void testBishopAlongDiagonals() {
    Board board(8, 8);
    Piece* bishop = placePiece(board, PieceColor::WHITE, PieceKind::BISHOP, Position(3, 3));

    BishopRule bishopRule;
    std::set<Position> dests = bishopRule.legal_destinations(board, *bishop);

    assert(dests.size() == 13);

    assert(dests.count(Position(7, 7)) == 1);
    assert(dests.count(Position(0, 0)) == 1);
    assert(dests.count(Position(6, 0)) == 1);
    assert(dests.count(Position(0, 6)) == 1);

    assert(dests.count(Position(3, 3)) == 0);

    std::cout << "testBishopAlongDiagonals passed!" << std::endl;
}

void testBishopFriendlyAndEnemyBlockers() {
    Board board(8, 8);
    Piece* bishop = placePiece(board, PieceColor::WHITE, PieceKind::BISHOP, Position(3, 3));
    placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(5, 5));
    placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(1, 1));

    BishopRule bishopRule;
    std::set<Position> dests = bishopRule.legal_destinations(board, *bishop);

    assert(dests.count(Position(4, 4)) == 1);
    assert(dests.count(Position(5, 5)) == 0);
    assert(dests.count(Position(6, 6)) == 0);

    assert(dests.count(Position(2, 2)) == 1);
    assert(dests.count(Position(1, 1)) == 1);
    assert(dests.count(Position(0, 0)) == 0);

    std::cout << "testBishopFriendlyAndEnemyBlockers passed!" << std::endl;
}

//Queen combines the moves of Rook and Bishop
void testQueenCombinesRookAndBishop() {
    Board board(8, 8);
    Piece* queen = placePiece(board, PieceColor::WHITE, PieceKind::QUEEN, Position(3, 3));

    QueenRule queenRule;
    std::set<Position> dests = queenRule.legal_destinations(board, *queen);

    assert(dests.size() == 27);

    assert(dests.count(Position(3, 7)) == 1);
    assert(dests.count(Position(7, 3)) == 1);

    assert(dests.count(Position(7, 7)) == 1);
    assert(dests.count(Position(0, 0)) == 1);
    assert(dests.count(Position(6, 0)) == 1);
    assert(dests.count(Position(0, 6)) == 1);

    assert(dests.count(Position(3, 3)) == 0);

    std::cout << "testQueenCombinesRookAndBishop passed!" << std::endl;
}

//Knight jump over blockers
void testKnightJumpingOverBlockers() {
    Board board(8, 8);
    Piece* knight = placePiece(board, PieceColor::WHITE, PieceKind::KNIGHT, Position(3, 3));

    // Blockers surrounding the knight to prove it can jump over them
    placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(3, 4));
    placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(4, 3));

    KnightRule knightRule;
    std::set<Position> dests = knightRule.legal_destinations(board, *knight);

    assert(dests.count(Position(4, 5)) == 1);
    assert(dests.count(Position(5, 4)) == 1);
    assert(dests.count(Position(5, 2)) == 1);
    assert(dests.count(Position(4, 1)) == 1);
    assert(dests.count(Position(2, 5)) == 1);
    assert(dests.count(Position(1, 4)) == 1);
    assert(dests.count(Position(1, 2)) == 1);
    assert(dests.count(Position(2, 1)) == 1);

    std::cout << "testKnightJumpingOverBlockers passed!" << std::endl;
}

void testKnightCannotLandOnFriendly() {
    Board board(8, 8);
    Piece* knight = placePiece(board, PieceColor::WHITE, PieceKind::KNIGHT, Position(3, 3));
    placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(1, 2));
    placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(1, 4));

    KnightRule knightRule;
    std::set<Position> dests = knightRule.legal_destinations(board, *knight);

    assert(dests.count(Position(1, 2)) == 0);
    assert(dests.count(Position(1, 4)) == 1);

    std::cout << "testKnightCannotLandOnFriendly passed!" << std::endl;
}

//King move one square in any direction
void testKingMovesOneSquare() {
    Board board(8, 8);
    Piece* king = placePiece(board, PieceColor::WHITE, PieceKind::KING, Position(3, 3));

    KingRule kingRule;
    std::set<Position> dests = kingRule.legal_destinations(board, *king);

    assert(dests.size() == 8);

    assert(dests.count(Position(2, 2)) == 1);
    assert(dests.count(Position(2, 3)) == 1);
    assert(dests.count(Position(2, 4)) == 1);
    assert(dests.count(Position(3, 2)) == 1);
    assert(dests.count(Position(3, 4)) == 1);
    assert(dests.count(Position(4, 2)) == 1);
    assert(dests.count(Position(4, 3)) == 1);
    assert(dests.count(Position(4, 4)) == 1);

    assert(dests.count(Position(3, 3)) == 0);

    std::cout << "testKingMovesOneSquare passed!" << std::endl;
}

//Pawn rules: single/double forward, blocked forward, diagonal captures
void testPawnMoves() {
    Board board(8, 8);
    Piece* pawn = placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(6, 3));
    placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(5, 2));
    placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(5, 4));

    PawnRule pawnRule;
    std::set<Position> dests = pawnRule.legal_destinations(board, *pawn);

    assert(dests.size() == 3);

    assert(dests.count(Position(5, 3)) == 1);
    assert(dests.count(Position(4, 3)) == 1);
    assert(dests.count(Position(5, 2)) == 0);
    assert(dests.count(Position(5, 4)) == 1);
    assert(dests.count(Position(6, 3)) == 0);

    std::cout << "testPawnMoves passed!" << std::endl;
}

void testPawnForwardBlockedAlsoBlocksDoubleMove() {
    Board board(8, 8);
    Piece* pawn = placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(6, 3));
    placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(5, 3));

    PawnRule pawnRule;
    std::set<Position> dests = pawnRule.legal_destinations(board, *pawn);

    assert(dests.count(Position(5, 3)) == 0);
    assert(dests.count(Position(4, 3)) == 0);
    assert(dests.size() == 0);

    std::cout << "testPawnForwardBlockedAlsoBlocksDoubleMove passed!" << std::endl;
}

void testPawnNotOnStartRowHasNoDoubleMove() {
    Board board(8, 8);
    Piece* pawn = placePiece(board, PieceColor::WHITE, PieceKind::PAWN, Position(4, 3));

    PawnRule pawnRule;
    std::set<Position> dests = pawnRule.legal_destinations(board, *pawn);

    assert(dests.count(Position(3, 3)) == 1);
    assert(dests.count(Position(2, 3)) == 0);
    assert(dests.size() == 1);

    std::cout << "testPawnNotOnStartRowHasNoDoubleMove passed!" << std::endl;
}

void testBlackPawnMovesDownward() {
    Board board(8, 8);
    Piece* pawn = placePiece(board, PieceColor::BLACK, PieceKind::PAWN, Position(1, 3));

    PawnRule pawnRule;
    std::set<Position> dests = pawnRule.legal_destinations(board, *pawn);

    assert(dests.count(Position(2, 3)) == 1);
    assert(dests.count(Position(3, 3)) == 1);
    assert(dests.size() == 2);

    std::cout << "testBlackPawnMovesDownward passed!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING PIECE RULE UNIT TESTS       " << std::endl;
    std::cout << "========================================" << std::endl;

    testRookEmptyBoard();
    testRookFriendlyBlocker();
    testRookEnemyBlocker();
    testBishopAlongDiagonals();
    testBishopFriendlyAndEnemyBlockers();
    testQueenCombinesRookAndBishop();
    testKnightJumpingOverBlockers();
    testKnightCannotLandOnFriendly();
    testKingMovesOneSquare();
    testPawnMoves();
    testPawnForwardBlockedAlsoBlocksDoubleMove();
    testPawnNotOnStartRowHasNoDoubleMove();
    testBlackPawnMovesDownward();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL TESTS EXECUTED                    " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
