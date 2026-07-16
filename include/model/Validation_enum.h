#pragma once
#include <string>

enum class MoveStatus {
    OK,
    OUTSIDE_BOARD,
    EMPTY_SOURCE,
    FRIENDLY_DESTINATION,
    ILLEGAL_PIECE_MOVE,
    GAME_OVER
};

inline std::string statusToString(MoveStatus status) {
    switch (status) {
        case MoveStatus::OK:                   return "ok";
        case MoveStatus::OUTSIDE_BOARD:        return "outside_board";
        case MoveStatus::EMPTY_SOURCE:         return "empty_source";
        case MoveStatus::FRIENDLY_DESTINATION: return "friendly_destination";
        case MoveStatus::ILLEGAL_PIECE_MOVE:   return "illegal_piece_move";
        case MoveStatus::GAME_OVER:            return "game_over";
        default:                               return "unknown_error";
    }
}