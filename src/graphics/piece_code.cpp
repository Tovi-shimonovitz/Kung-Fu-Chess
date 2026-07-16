#include "../../include/graphics/piece_code.h"
#include <stdexcept>

std::string PieceCode::shortCode(PieceColor color, PieceKind kind) {
    char kindChar;
    switch (kind) {
        case PieceKind::KING:   kindChar = 'K'; break;
        case PieceKind::QUEEN:  kindChar = 'Q'; break;
        case PieceKind::ROOK:   kindChar = 'R'; break;
        case PieceKind::BISHOP: kindChar = 'B'; break;
        case PieceKind::KNIGHT: kindChar = 'N'; break;
        case PieceKind::PAWN:   kindChar = 'P'; break;
        default: throw std::invalid_argument("PieceCode::shortCode: piece has no kind");
    }

    char colorChar;
    switch (color) {
        case PieceColor::WHITE: colorChar = 'W'; break;
        case PieceColor::BLACK: colorChar = 'B'; break;
        default: throw std::invalid_argument("PieceCode::shortCode: piece has no color");
    }

    return std::string(1, kindChar) + colorChar;
}

std::string PieceCode::stateFolderName(PieceState state) {
    switch (state) {
        case PieceState::IDLE:     return "idle";
        case PieceState::MOVING:   return "move";
        case PieceState::COOLDOWN: return "long_rest";
        default:
            throw std::invalid_argument("PieceCode::stateFolderName: no sprite folder for this state");
    }
}
