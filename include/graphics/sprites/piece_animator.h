#pragma once
#include <string>
#include "piece_animation_set.h"
#include "../../model/Piece_enums.h"

class PieceAnimator {
public:
    explicit PieceAnimator(PieceAnimationSet& animationSet);

    void syncToGameState(PieceState engineState);
    void advance(int elapsedMs);

    Img& currentFrame();
    const std::string& stateName() const { return activeStateName; }
    int frameIndex() const { return activeFrameIndex; }

private:
    void switchToState(const std::string& newStateName);

    PieceAnimationSet& animationSet;
    std::string activeStateName;
    int activeFrameIndex;
    int elapsedInFrameMs;
    PieceState lastGameState;
};
