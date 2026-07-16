#pragma once
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "Position.h"
#include "Piece.h"

class Board {
private:
    std::map<Position, std::unique_ptr<Piece>> pieces;

public:
    const int width;
    const int height;

    Board(int width, int height);

    bool isInsideBounds(const Position& pos) const;
    bool addPiece(const Position& pos, std::unique_ptr<Piece> piece);
    bool removePiece(const Position& pos);
    Piece* getPieceAt(const Position& pos) const;
    void movePiece(const Position& source, const Position& destination);
    std::vector<Piece*> getAllPieces() const;
};