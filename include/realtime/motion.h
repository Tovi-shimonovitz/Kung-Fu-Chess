#pragma once
#include <string>
#include "../model/Position.h"
#include "../model/Piece.h"

struct ExactPosition {
    double row;
    double col;
};

Position roundToSquare(const ExactPosition& position);

struct Motion {
    Piece* piece;
    Position source;
    Position target;
    int remainingTimeMs;

    Motion(Piece* p, Position src, Position dst, int duration)
        : piece(p), source(src), target(dst), remainingTimeMs(duration) {}

    bool isFinished() const { return remainingTimeMs <= 0; }

    static int calculateDuration(Position src, Position dst);
    int totalDurationMs() const;
    int elapsedMs() const;
    ExactPosition positionAtElapsed(int elapsedMs) const;
    ExactPosition currentPosition() const;
};