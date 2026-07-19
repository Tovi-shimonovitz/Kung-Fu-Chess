#include "../../../include/graphics/sprites/piece_animation_set.h"
#include <stdexcept>
#include <vector>

namespace {
const std::vector<std::string> kStateNames = {
    "idle", "move", "jump", "long_rest", "short_rest"
};
}

PieceAnimationSet PieceAnimationSet::loadFromFolder(const std::string& pieceFolderPath) {
    PieceAnimationSet set;

    for (const std::string& stateName : kStateNames) {
        const std::string stateFolderPath = pieceFolderPath + "/states/" + stateName;
        set.clips.emplace(stateName, AnimationClip::loadFromFolder(stateFolderPath));
    }

    return set;
}

AnimationClip& PieceAnimationSet::clipFor(const std::string& stateName) {
    auto it = clips.find(stateName);
    if (it == clips.end()) {
        throw std::invalid_argument("PieceAnimationSet: unknown state \"" + stateName + "\"");
    }
    return it->second;
}
