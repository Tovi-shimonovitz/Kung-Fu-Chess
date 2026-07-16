#pragma once
#include <memory>
#include "../../include/model/Piece.h"
#include "../../include/model/Piece_enums.h"
#include "../../include/model/Position.h"
#include "../../include/model/PieceFactory.h"
#include <stdexcept>



 std::unique_ptr<Piece> PieceFactory::createPiece(PieceColor color, PieceKind type, Position pos) {
    if (type == PieceKind::None || color == PieceColor::None) {
        return nullptr;
    }
    auto piece = std::make_unique<Piece>(color, type);
    
    piece->setPosition(pos);
    return piece;          
}
