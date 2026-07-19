#pragma once
#include <string>
#include <unordered_map>

class EnvConfig {
public:
    static EnvConfig load(const std::string& path);

    const std::string& get(const std::string& key) const;
    int getInt(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> values;
};
