#pragma once
#include "Position.h"
#include <memory>
#include "Piece.h"
#include "Piece_enums.h"

class PieceFactory {
public:
    static std::unique_ptr<Piece> createPiece(PieceColor color, PieceKind kind, Position pos);
};
