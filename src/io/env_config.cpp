#include "../../include/io/env_config.h"
#include <fstream>
#include <stdexcept>

EnvConfig EnvConfig::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("ERROR CANNOT_OPEN_ENV_FILE: " + path);
    }

    EnvConfig config;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        config.values[line.substr(0, eq)] = line.substr(eq + 1);
    }
    return config;
}

const std::string& EnvConfig::get(const std::string& key) const {
    auto it = values.find(key);
    if (it == values.end()) {
        throw std::runtime_error("ERROR MISSING_ENV_KEY: " + key);
    }
    return it->second;
}

int EnvConfig::getInt(const std::string& key) const {
    return std::stoi(get(key));
}
