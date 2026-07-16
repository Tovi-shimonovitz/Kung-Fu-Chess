#pragma once
#include "../../include/model/Piece.h"
#include <cctype>



void Piece::setPosition(const Position& pos) {
    position = pos;
}

std::string Piece::toString() const {
    std::string colorName = (color == PieceColor::WHITE) ? "white" : "black";
    std::string pieceName;

    switch (kind) {
        case PieceKind::KING:   pieceName = "king"; break;
        case PieceKind::QUEEN:  pieceName = "queen"; break;
        case PieceKind::ROOK:   pieceName = "rook"; break;
        case PieceKind::BISHOP: pieceName = "bishop"; break;
        case PieceKind::KNIGHT: pieceName = "knight"; break;
        case PieceKind::PAWN:   pieceName = "pawn"; break;
    }

    return colorName + " " + pieceName;
}