#pragma once
#include <memory>
#include <set>
#include "../model/Board.h"
#include "../model/Piece.h"
#include "../model/Position.h"

class PieceRule {
public:
    virtual ~PieceRule() = default;
    virtual std::set<Position> legal_destinations(const Board& board, const Piece& piece) const = 0;
};

class RookRule: public PieceRule {
public:
    RookRule() = default;
    ~RookRule() = default;
    std::set<Position> legal_destinations(const Board& board, const Piece& rook) const override;
};

class BishopRule: public PieceRule {
public:
    BishopRule() = default;
    ~BishopRule() = default;
    std::set<Position> legal_destinations(const Board& board, const Piece& bishop) const override;
};

class QueenRule: public PieceRule {
public:
    QueenRule() = default;
    ~QueenRule() = default;
    std::set<Position> legal_destinations(const Board& board, const Piece& queen) const override;
};

class KnightRule: public PieceRule {
public:
    KnightRule() = default;
    ~KnightRule() = default;
    std::set<Position> legal_destinations(const Board& board, const Piece& knight) const override;
};

class KingRule: public PieceRule {
public:
    KingRule() = default;
    ~KingRule() = default;
    std::set<Position> legal_destinations(const Board& board, const Piece& king) const override;
};

class PawnRule: public PieceRule {
public:
    PawnRule() = default;
    ~PawnRule() = default;
    std::set<Position> legal_destinations(const Board& board, const Piece& pawn) const override;
};

class RuleFactory {
public:
    static std::unique_ptr<PieceRule> createRule(PieceKind kind) {
        switch (kind) {
            case PieceKind::ROOK:   return std::make_unique<RookRule>();
            case PieceKind::BISHOP: return std::make_unique<BishopRule>();
            case PieceKind::KNIGHT: return std::make_unique<KnightRule>();
            case PieceKind::QUEEN:  return std::make_unique<QueenRule>();
            case PieceKind::KING:   return std::make_unique<KingRule>();
            case PieceKind::PAWN:   return std::make_unique<PawnRule>();
            default: return nullptr;
        }
    }
};