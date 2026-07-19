#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include "../../include/graphics/sprites/animation_config.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void writeFixture(const std::string& path, const std::string& contents) {
    std::ofstream file(path);
    file << contents;
}

void test_load_looping_idle_style_config() {
    const std::string path = "test_animation_config_idle.json";
    writeFixture(path, R"({
  "physics": {
    "speed_m_per_sec": 0.0,
    "next_state_when_finished": "idle"
  },
  "graphics": {
    "frames_per_sec": 6,
    "is_loop": true
  }
})");

    AnimationConfig config = AnimationConfig::loadFromFile(path);
    assert(config.speedMps == 0.0);
    assert(config.nextStateWhenFinished == "idle");
    assert(config.framesPerSec == 6);
    assert(config.isLoop == true);

    std::remove(path.c_str());
    log_test("test_load_looping_idle_style_config");
}

void test_load_finite_move_style_config() {
    const std::string path = "test_animation_config_move.json";
    writeFixture(path, R"({
  "physics": {
    "speed_m_per_sec": 1.5,
    "next_state_when_finished": "long_rest"
  },
  "graphics": {
    "frames_per_sec": 12,
    "is_loop": false
  }
})");

    AnimationConfig config = AnimationConfig::loadFromFile(path);
    assert(config.speedMps == 1.5);
    assert(config.nextStateWhenFinished == "long_rest");
    assert(config.framesPerSec == 12);
    assert(config.isLoop == false);

    std::remove(path.c_str());
    log_test("test_load_finite_move_style_config");
}

void test_missing_file_throws() {
    bool threw = false;
    try {
        AnimationConfig::loadFromFile("does_not_exist.json");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);
    log_test("test_missing_file_throws");
}

int main() {
    std::cout << "--- Starting AnimationConfig Unit Tests ---" << std::endl;

    test_load_looping_idle_style_config();
    test_load_finite_move_style_config();
    test_missing_file_throws();

    std::cout << "--- All AnimationConfig tests passed successfully ---" << std::endl;
    return 0;
}
