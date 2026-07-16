#include "../../include/graphics/piece_animator.h"
#include "../../include/graphics/piece_code.h"

PieceAnimator::PieceAnimator(PieceAnimationSet& animationSet)
    : animationSet(animationSet),
      activeStateName(PieceCode::stateFolderName(PieceState::IDLE)),
      activeFrameIndex(0),
      elapsedInFrameMs(0),
      lastGameState(PieceState::IDLE) {}

void PieceAnimator::switchToState(const std::string& newStateName) {
    activeStateName = newStateName;
    activeFrameIndex = 0;
    elapsedInFrameMs = 0;
}

void PieceAnimator::syncToGameState(PieceState engineState) {
    if (engineState != lastGameState) {
        switchToState(PieceCode::stateFolderName(engineState));
        lastGameState = engineState;
    }
}

void PieceAnimator::advance(int elapsedMs) {
    AnimationClip& clip = animationSet.clipFor(activeStateName);
    const int frameDurationMs = 1000 / clip.config().framesPerSec;

    elapsedInFrameMs += elapsedMs;

    while (elapsedInFrameMs >= frameDurationMs) {
        elapsedInFrameMs -= frameDurationMs;

        bool onLastFrame = (activeFrameIndex == clip.frameCount() - 1);
        if (onLastFrame) {
            if (clip.config().isLoop) {
                activeFrameIndex = 0;
            }
            // not looping: freeze on the last frame, don't advance further
        } else {
            ++activeFrameIndex;
        }
    }
}

Img& PieceAnimator::currentFrame() {
    return animationSet.clipFor(activeStateName).frameAt(activeFrameIndex);
}
