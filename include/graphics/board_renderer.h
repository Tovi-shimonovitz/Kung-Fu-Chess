#pragma once
#include <string>
#include <unordered_map>
#include "Img.h"
#include "sprite_repository.h"
#include "piece_animator.h"
#include "../engine/Result_structs.h"

class BoardRenderer {
public:
    BoardRenderer(std::string boardImagePath, SpriteRepository& spriteRepository);

    Img render(const GameSnapshot& snapshot, int elapsedMs);

private:
    PieceAnimator& animatorFor(Piece* piece);
    void removeStaleAnimators(const GameSnapshot& snapshot);

    std::string boardImagePath;
    SpriteRepository& spriteRepository;
    std::unordered_map<Piece*, PieceAnimator> animators;
};
