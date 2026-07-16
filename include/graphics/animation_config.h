#pragma once
#include <string>

struct AnimationConfig {
    double speedMps;
    std::string nextStateWhenFinished;
    int framesPerSec;
    bool isLoop;

    static AnimationConfig loadFromFile(const std::string& path);
};
