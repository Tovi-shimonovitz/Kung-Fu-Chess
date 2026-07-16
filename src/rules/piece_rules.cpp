#pragma once
#include "../../include/rules/piece_rules.h"
#include <set>
#include <iostream>
#include "../../include/model/Piece.h"

namespace {

bool tryAddDestination(const Board& board, const Piece& piece, const Position& nextPos,
                       std::set<Position>& destinations, bool allowEmpty = true) {
    if (!board.isInsideBounds(nextPos)) {
        return false;
    }

    const auto occupyingPiece = board.getPieceAt(nextPos);
    if (occupyingPiece == nullptr) {
        if (allowEmpty) {
            destinations.insert(nextPos);
        }
        return false;
    }

    if (occupyingPiece->color != piece.color) {
        destinations.insert(nextPos);
    }

    return true;
}
}

std::set<Position> RookRule::legal_destinations(const Board& board, const Piece& piece) const {
    std::set<Position> destinations;

    Position currentPos = piece.position; 

   //options of steps (up, down, left, right)
    const int directions[4][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    for (int i = 0; i < 4; ++i) {
        int dRow = directions[i][0];
        int dCol = directions[i][1];

        int nextRow = currentPos.row + dRow;
        int nextCol = currentPos.col + dCol;
        Position nextPos(nextRow, nextCol);

        while (board.isInsideBounds(nextPos)) {
            if (tryAddDestination(board, piece, nextPos, destinations)) {
                break;
            }

            nextRow += dRow;
            nextCol += dCol;
            nextPos = Position(nextRow, nextCol);
        }
    }

    return destinations;
}

std::set<Position> BishopRule::legal_destinations(const Board& board, const Piece& piece) const {
    std::set<Position> destinations;

    Position currentPos = piece.position;

    const int directions[4][2] = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    for (int i = 0; i < 4; ++i) {
        int dRow = directions[i][0];
        int dCol = directions[i][1];

        int nextRow = currentPos.row + dRow;
        int nextCol = currentPos.col + dCol;
        Position nextPos(nextRow, nextCol);

        while (board.isInsideBounds(nextPos)) {
            if (tryAddDestination(board, piece, nextPos, destinations)) {
                break;
            }

            nextRow += dRow;
            nextCol += dCol;
            nextPos = Position(nextRow, nextCol);
        }
    }

    return destinations;
}

std::set<Position> QueenRule::legal_destinations(const Board& board, const Piece& piece) const {
    std::set<Position> destinations;

    RookRule rookRule;
    BishopRule bishopRule;

    auto rookDestinations = rookRule.legal_destinations(board, piece);
    auto bishopDestinations = bishopRule.legal_destinations(board, piece);

    destinations.insert(rookDestinations.begin(), rookDestinations.end());
    destinations.insert(bishopDestinations.begin(), bishopDestinations.end());

    return destinations;
}

std::set<Position> KnightRule::legal_destinations(const Board& board, const Piece& piece) const {
    std::set<Position> destinations;

    Position currentPos = piece.position;

    const int knightMoves[8][2] = {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        {1, -2}, {1, 2},
        {2, -1}, {2, 1}
    };

    for (int i = 0; i < 8; ++i) {
        Position nextPos(currentPos.row + knightMoves[i][0], currentPos.col + knightMoves[i][1]);
        tryAddDestination(board, piece, nextPos, destinations);
    }

    return destinations;
}

std::set<Position> KingRule::legal_destinations(const Board& board, const Piece& piece) const {
    std::set<Position> destinations;

    Position currentPos = piece.position;

    const int kingMoves[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (int i = 0; i < 8; ++i) {
        Position nextPos(currentPos.row + kingMoves[i][0], currentPos.col + kingMoves[i][1]);
        tryAddDestination(board, piece, nextPos, destinations);
    }

    return destinations;
}

std::set<Position> PawnRule::legal_destinations(const Board& board, const Piece& piece) const {
    std::set<Position> destinations;

    Position currentPos = piece.position;
    int direction = (piece.color == PieceColor::WHITE) ? -1 : 1;

    Position forwardPos(currentPos.row + direction, currentPos.col);
    if (board.isInsideBounds(forwardPos) && board.getPieceAt(forwardPos) == nullptr) {
        destinations.insert(forwardPos);

        if ((piece.color == PieceColor::WHITE && currentPos.row == 6) ||
            (piece.color == PieceColor::BLACK && currentPos.row == 1)) {
            Position doubleForwardPos(currentPos.row + 2 * direction, currentPos.col);
            if (board.isInsideBounds(doubleForwardPos) && board.getPieceAt(doubleForwardPos) == nullptr) {
                destinations.insert(doubleForwardPos);
            }
        }
    }

    Position captureLeft(currentPos.row + direction, currentPos.col - 1);
    Position captureRight(currentPos.row + direction, currentPos.col + 1);

    tryAddDestination(board, piece, captureLeft, destinations, false);
    tryAddDestination(board, piece, captureRight, destinations, false);

    return destinations;
}