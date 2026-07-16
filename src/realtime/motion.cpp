#pragma once
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "../../include/realtime/motion.h"

int Motion::calculateDuration(Position src, Position dst) {
    int steps = std::max(std::abs(src.row - dst.row), std::abs(src.col - dst.col));
    return steps * 1000;
}

int Motion::totalDurationMs() const {
    return calculateDuration(source, target);
}

int Motion::elapsedMs() const {
    return totalDurationMs() - remainingTimeMs;
}

ExactPosition Motion::positionAtElapsed(int elapsedMs) const {
    double progress = static_cast<double>(elapsedMs) / static_cast<double>(totalDurationMs());

    return ExactPosition{
        source.row + (target.row - source.row) * progress,
        source.col + (target.col - source.col) * progress
    };
}

ExactPosition Motion::currentPosition() const {
    return positionAtElapsed(elapsedMs());
}

Position roundToSquare(const ExactPosition& position) {
    return Position(
        static_cast<int>(std::lround(position.row)),
        static_cast<int>(std::lround(position.col))
    );
}
