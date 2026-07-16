#pragma once
#include "motion.h"

enum class CollisionOutcome {
    NONE,
    CAPTURE,
    BLOCK
};

struct CollisionDecision {
    CollisionOutcome outcome = CollisionOutcome::NONE;
    const Motion* earlier = nullptr;
    const Motion* later = nullptr;
};

class CollisionResolver {
public:
    virtual ~CollisionResolver() = default;
    virtual CollisionDecision resolve(const Motion& first, const Motion& second, int tickMs) const = 0;
    virtual CollisionOutcome resolveAgainstStationary(const Motion& moving, const Piece& stationary) const = 0;
};

class NullCollisionResolver : public CollisionResolver {
public:
    CollisionDecision resolve(const Motion& first, const Motion& second, int tickMs) const override {
        return CollisionDecision{};
    }

    CollisionOutcome resolveAgainstStationary(const Motion& moving, const Piece& stationary) const override {
        return CollisionOutcome::NONE;
    }
};
