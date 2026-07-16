#include "../../include/graphics/sprite_repository.h"
#include "../../include/graphics/piece_code.h"
#include <vector>

SpriteRepository::SpriteRepository(std::string basePath) : basePath(std::move(basePath)) {}

PieceAnimationSet& SpriteRepository::get(PieceColor color, PieceKind kind) {
    const std::string code = PieceCode::shortCode(color, kind);

    auto found = cache.find(code);
    if (found != cache.end()) {
        return found->second;
    }

    auto inserted = cache.emplace(code, PieceAnimationSet::loadFromFolder(basePath + "/" + code));
    return inserted.first->second;
}

void SpriteRepository::preloadAll() {
    static const std::vector<PieceColor> allColors = { PieceColor::WHITE, PieceColor::BLACK };
    static const std::vector<PieceKind> allKinds = {
        PieceKind::KING, PieceKind::QUEEN, PieceKind::ROOK,
        PieceKind::BISHOP, PieceKind::KNIGHT, PieceKind::PAWN
    };

    for (PieceColor color : allColors) {
        for (PieceKind kind : allKinds) {
            get(color, kind);
        }
    }
}
