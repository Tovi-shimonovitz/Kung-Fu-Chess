#pragma once
#include "collision_resolver.h"

class StandardCollisionResolver : public CollisionResolver {
public:
    CollisionDecision resolve(const Motion& first, const Motion& second, int tickMs) const override;
    CollisionOutcome resolveAgainstStationary(const Motion& moving, const Piece& stationary) const override;

private:
    CollisionDecision resolveRivals(const Motion& first, const Motion& second) const;
    CollisionDecision resolveSameColor(const Motion& first, const Motion& second, int tickMs) const;

    static double penetrationMs(const Motion& motion);
    static bool wasAlreadyOnSquare(const Motion& motion, int tickMs, Position square);
};
