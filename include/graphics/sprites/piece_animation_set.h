#pragma once
#include <string>
#include <unordered_map>
#include "animation_clip.h"

class PieceAnimationSet {
public:
    static PieceAnimationSet loadFromFolder(const std::string& pieceFolderPath);

    AnimationClip& clipFor(const std::string& stateName);

private:
    std::unordered_map<std::string, AnimationClip> clips;
};
