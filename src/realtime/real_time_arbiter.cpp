#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "../../include/realtime/motion.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/engine/Result_structs.h"
#include "../../include/realtime/real_time_arbiter.h"
#include "../../include/realtime/standard_collision_resolver.h"

namespace {
bool isPromotionRank(const Position& pos, PieceColor color) {
    return (color == PieceColor::WHITE && pos.row == 0) ||
           (color == PieceColor::BLACK && pos.row == 7);
}
}

RealTimeArbiter::RealTimeArbiter(std::unique_ptr<CollisionResolver> resolver)
    : collisionResolver(resolver ? std::move(resolver) : std::make_unique<StandardCollisionResolver>())
{}

bool RealTimeArbiter::hasActiveMotion() const { return !activeMotions.empty(); }

const std::vector<Motion>& RealTimeArbiter::activeMotionsView() const { return activeMotions; }

void RealTimeArbiter::startMotion(Piece* piece, Position src, Position dst) {
    int duration = Motion::calculateDuration(src, dst);
    activeMotions.emplace_back(piece, src, dst, duration);
}

WaitResult RealTimeArbiter::advanceTime(int ms, Board& board) {
    WaitResult result{false, ""};

    while (ms > 0) {
        int tickMs = std::min(ms, TICK_MS);
        processTick(tickMs, board, result);
        ms -= tickMs;
    }

    return result;
}

void RealTimeArbiter::processTick(int tickMs, Board& board, WaitResult& result) {
    for (auto& motion : activeMotions) {
        motion.remainingTimeMs -= tickMs;
    }

    std::vector<RedirectedArrival> redirects = detectCollisions(tickMs, board);
    if (!redirects.empty()) {
        applyRedirects(redirects, board, result);
        result.was_processed = true;
    }

    for (auto it = activeMotions.begin(); it != activeMotions.end(); ) {
        if (it->isFinished()) {
            performArrival(*it, board, result);
            result.was_processed = true;
            it = activeMotions.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = cooldowns.begin(); it != cooldowns.end(); ) {
    it->remainingMs -= tickMs;
    if (it->remainingMs <= 0) {
        it->piece->state = PieceState::IDLE;
        result.was_processed = true;   
        it = cooldowns.erase(it);
    } else {
        ++it;
    }
}
}

bool RealTimeArbiter::isCurrentlyMoving(const Piece* piece) const {
    for (const auto& motion : activeMotions) {
        if (motion.piece == piece) {
            return true;
        }
    }
    return false;
}

std::vector<RealTimeArbiter::RedirectedArrival> RealTimeArbiter::detectCollisions(int tickMs, Board& board) const {
    std::vector<RedirectedArrival> redirects;

    for (size_t i = 0; i < activeMotions.size(); ++i) {
        const Motion& motion = activeMotions[i];
        if (motion.piece->kind == PieceKind::KNIGHT) {
        continue;   
        }
        Position currentSquare = roundToSquare(motion.currentPosition());

        for (size_t j = i + 1; j < activeMotions.size(); ++j) {
            const Motion& other = activeMotions[j];
            if (other.piece->kind == PieceKind::KNIGHT) {
                continue;   
            }
            if (!(roundToSquare(other.currentPosition()) == currentSquare)) {
                continue;
            }

            CollisionDecision decision = collisionResolver->resolve(motion, other, tickMs);
            appendMotionRedirects(decision, tickMs, redirects);
        }

        Piece* stationary = board.getPieceAt(currentSquare);

        if ( stationary != nullptr && 
            stationary != motion.piece
             && !isCurrentlyMoving(stationary)) {
            CollisionOutcome outcome = collisionResolver->resolveAgainstStationary(motion, *stationary);
            appendStationaryRedirect(outcome, motion, currentSquare, tickMs, redirects);
        }
    }

    return redirects;
}

void RealTimeArbiter::appendMotionRedirects(const CollisionDecision& decision, int tickMs,
                                             std::vector<RedirectedArrival>& redirects) const {
    if (decision.outcome == CollisionOutcome::NONE) {
        return;
    }

    if (decision.outcome == CollisionOutcome::CAPTURE) {
        Position collisionSquare = roundToSquare(decision.earlier->currentPosition());
        redirects.push_back({ decision.earlier, collisionSquare });
        redirects.push_back({ decision.later, collisionSquare });
        return;
    }

    Position previousSquare = roundToSquare(
        decision.later->positionAtElapsed(decision.later->elapsedMs() - tickMs));
    redirects.push_back({ decision.later, previousSquare });
}

void RealTimeArbiter::appendStationaryRedirect(CollisionOutcome outcome, const Motion& moving, Position currentSquare,
                                                int tickMs, std::vector<RedirectedArrival>& redirects) const {
    if (outcome == CollisionOutcome::NONE) {
        return;
    }

    if (outcome == CollisionOutcome::CAPTURE) {
        redirects.push_back({ &moving, currentSquare });
        return;
    }

    Position previousSquare = roundToSquare(moving.positionAtElapsed(moving.elapsedMs() - tickMs));
    redirects.push_back({ &moving, previousSquare });
}

void RealTimeArbiter::applyRedirects(const std::vector<RedirectedArrival>& redirects, Board& board, WaitResult& result) {
    std::vector<Piece*> resolvedPieces;

    for (const auto& redirect : redirects) {
        Motion synthetic(redirect.motion->piece, redirect.motion->source, redirect.target, 0);
        performArrival(synthetic, board, result);
        resolvedPieces.push_back(redirect.motion->piece);
    }

    activeMotions.erase(
        std::remove_if(activeMotions.begin(), activeMotions.end(),
            [&resolvedPieces](const Motion& motion) {
                return std::find(resolvedPieces.begin(), resolvedPieces.end(), motion.piece) != resolvedPieces.end();
            }),
        activeMotions.end());
}

void RealTimeArbiter::performArrival(const Motion& m, Board& board, WaitResult& result) {
        
    auto pieceAtTarget = board.getPieceAt(m.target);

    bool isKingEaten = pieceAtTarget != nullptr && pieceAtTarget->kind == PieceKind::KING;

    if (pieceAtTarget != nullptr) {
        cooldowns.erase(
            std::remove_if(cooldowns.begin(), cooldowns.end(),
                [pieceAtTarget](const Cooldown& c) { return c.piece == pieceAtTarget; }),
            cooldowns.end());
    }
    
    board.movePiece(m.source, m.target);

    Piece* arrived = m.piece;
    if (arrived->kind == PieceKind::PAWN && isPromotionRank(m.target, arrived->color)) {
        arrived = board.replacePiece(m.target,
            PieceFactory::createPiece(arrived->color, PieceKind::QUEEN, m.target));
    }

    arrived->state = PieceState::COOLDOWN;
    cooldowns.push_back({ arrived, m.totalDurationMs() });

    if (isKingEaten) {
        result.message = "king_eaten";
    }
    result.message += "motion_completed: " +
    m.source.toString() + "->" + m.target.toString() + " ";
}
