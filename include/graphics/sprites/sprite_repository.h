#pragma once
#include <string>
#include <unordered_map>
#include "piece_animation_set.h"
#include "../../model/Piece_enums.h"

class SpriteRepository {
public:
    explicit SpriteRepository(std::string basePath);

    PieceAnimationSet& get(PieceColor color, PieceKind kind);
    void preloadAll();

private:
    std::string basePath;
    std::unordered_map<std::string, PieceAnimationSet> cache;
};
