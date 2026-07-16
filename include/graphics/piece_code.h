#pragma once
#include <string>
#include "../model/Piece_enums.h"

class PieceCode {
public:
    static std::string shortCode(PieceColor color, PieceKind kind);
    static std::string stateFolderName(PieceState state);
};
