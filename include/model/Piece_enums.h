#pragma once

enum class PieceColor {
    None,
    WHITE,
    BLACK
};

enum class PieceKind {
    None,
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN
};

enum class PieceState {
    IDLE,
    MOVING,
    COOLDOWN,
    CAPTURED
};