#include "../../../include/graphics/sprites/animation_clip.h"
#include <filesystem>
#include <stdexcept>

AnimationClip AnimationClip::loadFromFolder(const std::string& stateFolderPath) {
    AnimationClip clip;
    clip.clipConfig = AnimationConfig::loadFromFile(stateFolderPath + "/config.json");

    const std::string spritesFolder = stateFolderPath + "/sprites/";
    int frameNumber = 1;

    while (std::filesystem::exists(spritesFolder + std::to_string(frameNumber) + ".png")) {
        Img frame;
        frame.read(spritesFolder + std::to_string(frameNumber) + ".png");
        clip.frames.push_back(frame);
        ++frameNumber;
    }

    if (clip.frames.empty()) {
        throw std::runtime_error("AnimationClip: no sprite frames found in " + spritesFolder);
    }

    return clip;
}

Img& AnimationClip::frameAt(int index) {
    if (index < 0 || index >= static_cast<int>(frames.size())) {
        throw std::out_of_range("AnimationClip: frame index out of range");
    }
    return frames[index];
}
