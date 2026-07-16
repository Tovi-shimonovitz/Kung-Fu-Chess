#pragma once
#include <cmath>
#include "../../include/realtime/standard_collision_resolver.h"

CollisionDecision StandardCollisionResolver::resolve(const Motion& first, const Motion& second, int tickMs) const {
    if (first.piece->color == second.piece->color) {
        return resolveSameColor(first, second, tickMs);
    }
    return resolveRivals(first, second);
}

CollisionOutcome StandardCollisionResolver::resolveAgainstStationary(const Motion& moving, const Piece& stationary) const {
    return moving.piece->color == stationary.color ? CollisionOutcome::BLOCK : CollisionOutcome::CAPTURE;
}

CollisionDecision StandardCollisionResolver::resolveRivals(const Motion& first, const Motion& second) const {
    const Motion* earlier = &first;
    const Motion* later = &second;

    if (penetrationMs(second) > penetrationMs(first)) {
        earlier = &second;
        later = &first;
    }

    return CollisionDecision{ CollisionOutcome::CAPTURE, earlier, later };
}

CollisionDecision StandardCollisionResolver::resolveSameColor(const Motion& first, const Motion& second, int tickMs) const {
    Position sharedSquare = roundToSquare(first.currentPosition());

    bool firstWasAlreadyThere = wasAlreadyOnSquare(first, tickMs, sharedSquare);
    bool secondWasAlreadyThere = wasAlreadyOnSquare(second, tickMs, sharedSquare);

    if (firstWasAlreadyThere && !secondWasAlreadyThere) {
        return CollisionDecision{ CollisionOutcome::BLOCK, &first, &second };
    }
    if (secondWasAlreadyThere && !firstWasAlreadyThere) {
        return CollisionDecision{ CollisionOutcome::BLOCK, &second, &first };
    }

    const Motion* earlier = &first;
    const Motion* later = &second;

    if (second.elapsedMs() > first.elapsedMs()) {
        earlier = &second;
        later = &first;
    }

    return CollisionDecision{ CollisionOutcome::BLOCK, earlier, later };
}

double StandardCollisionResolver::penetrationMs(const Motion& motion) {
    double squareIndex = std::round(motion.elapsedMs() / 1000.0);
    return motion.elapsedMs() - (squareIndex - 0.5) * 1000.0;
}

bool StandardCollisionResolver::wasAlreadyOnSquare(const Motion& motion, int tickMs, Position square) {
    int previousElapsedMs = motion.elapsedMs() - tickMs;

    if (previousElapsedMs < 0) {
        return false;
    }

    return roundToSquare(motion.positionAtElapsed(previousElapsedMs)) == square;
}
