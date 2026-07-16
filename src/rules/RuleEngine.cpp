#pragma once
#include "../../include/rules/RuleEngine.h"
#include "../../include/rules/piece_rules.h"

MoveValidation RuleEngine::validateMove(const Board& board, Position source, Position target) const {
    if (!board.isInsideBounds(source) || !board.isInsideBounds(target)) {
        return MoveValidation::error(MoveStatus::OUTSIDE_BOARD);
    }

    auto sourcePiece = board.getPieceAt(source);
    if (sourcePiece == nullptr) {
        return MoveValidation::error(MoveStatus::EMPTY_SOURCE);
    }

    auto targetPiece = board.getPieceAt(target);
    if (targetPiece != nullptr && targetPiece->color == sourcePiece->color) {
        return MoveValidation::error(MoveStatus::FRIENDLY_DESTINATION);
    }
    
    auto rule = RuleFactory::createRule(sourcePiece->kind);
    if(rule){
        std::set<Position> legalMoves = rule->legal_destinations(board, *sourcePiece);
        if (legalMoves.find(target) == legalMoves.end()) {
            return MoveValidation::error(MoveStatus::ILLEGAL_PIECE_MOVE);
        }
    }
    return MoveValidation::ok();
}