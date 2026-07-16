#pragma once
#include <memory>
#include <vector>
#include "motion.h"
#include "collision_resolver.h"
#include "../model/Board.h"
#include "../engine/Result_structs.h"

class GameEngine;

class RealTimeArbiter {
public:
    static constexpr int TICK_MS = 50;

    explicit RealTimeArbiter(std::unique_ptr<CollisionResolver> resolver = nullptr);

    bool hasActiveMotion() const;

    void startMotion(Piece* piece, Position start, Position end);

    WaitResult advanceTime(int ms, Board& board);

    const std::vector<Motion>& activeMotionsView() const;

private:
    struct RedirectedArrival {
        const Motion* motion;
        Position target;
    };

    std::vector<Motion> activeMotions;
    std::unique_ptr<CollisionResolver> collisionResolver;

    void performArrival(const Motion& m, Board& board,WaitResult &result);
    void processTick(int tickMs, Board& board, WaitResult& result);

    bool isCurrentlyMoving(const Piece* piece) const;
    std::vector<RedirectedArrival> detectCollisions(int tickMs, Board& board) const;
    void appendMotionRedirects(const CollisionDecision& decision, int tickMs, std::vector<RedirectedArrival>& redirects) const;
    void appendStationaryRedirect(CollisionOutcome outcome, const Motion& moving, Position currentSquare, int tickMs, std::vector<RedirectedArrival>& redirects) const;
    void applyRedirects(const std::vector<RedirectedArrival>& redirects, Board& board, WaitResult& result);
};