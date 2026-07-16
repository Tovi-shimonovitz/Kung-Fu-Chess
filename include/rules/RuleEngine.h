#pragma once
#include <memory>
#include "../model/Board.h"
#include "../model/Piece.h"
#include "piece_rules.h"
#include <string>
#include "../model/Validation_enum.h"

struct MoveValidation {
    bool is_valid;
    MoveStatus reason;

    static MoveValidation ok() { return {true, MoveStatus::OK}; }
    static MoveValidation error(MoveStatus s) { return {false, s}; }

    std::string getReason() const {
        return statusToString(reason);
    }
};

class RuleEngine {
public:
    RuleEngine() = default;
    MoveValidation validateMove(const Board& board, Position source, Position target) const;
};
