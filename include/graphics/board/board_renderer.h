#pragma once
#include <string>
#include <unordered_map>
#include "../Img.h"
#include "../sprites/sprite_repository.h"
#include "../sprites/piece_animator.h"
#include "../../engine/Result_structs.h"

class BoardRenderer {
public:
    BoardRenderer(std::string boardImagePath, SpriteRepository& spriteRepository);

    Img render(const GameSnapshot& snapshot, int elapsedMs);

private:
    PieceAnimator& animatorFor(const std::string& id, PieceColor color, PieceKind kind);
    void removeStaleAnimators(const GameSnapshot& snapshot);

    std::string boardImagePath;
    SpriteRepository& spriteRepository;
    Img boardImage;
    std::unordered_map<std::string, PieceAnimator> animators;
};
