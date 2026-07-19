#include "../../../include/graphics/sprites/animation_config.h"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

AnimationConfig AnimationConfig::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("AnimationConfig: cannot open " + path);
    }

    nlohmann::json data;
    file >> data;

    AnimationConfig config;
    config.speedMps = data.at("physics").at("speed_m_per_sec").get<double>();
    config.nextStateWhenFinished = data.at("physics").at("next_state_when_finished").get<std::string>();
    config.framesPerSec = data.at("graphics").at("frames_per_sec").get<int>();
    config.isLoop = data.at("graphics").at("is_loop").get<bool>();

    return config;
}
