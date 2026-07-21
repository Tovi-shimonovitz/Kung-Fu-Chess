#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../model/Position.h"
#include "../model/Piece_enums.h"
#include "../realtime/motion.h"

struct MoveResult {
    bool is_accepted;
    std::string reason;
};

struct WaitResult {
    bool was_processed;
    std::string message;
};

struct MovingPiece {
    std::string id;
    PieceColor color;
    PieceKind kind;
    PieceState state;
    ExactPosition currentPos;
};

struct GameSnapshot {
    int width;
    int height;
    std::vector<MovingPiece> pieces;
    bool isGameOver;
};


