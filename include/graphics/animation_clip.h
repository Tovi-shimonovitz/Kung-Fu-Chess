#pragma once
#include <string>
#include <vector>
#include "Img.h"
#include "animation_config.h"

class AnimationClip {
public:
    static AnimationClip loadFromFolder(const std::string& stateFolderPath);

    const AnimationConfig& config() const { return clipConfig; }
    int frameCount() const { return static_cast<int>(frames.size()); }
    Img& frameAt(int index);

private:
    AnimationConfig clipConfig;
    std::vector<Img> frames;
};
