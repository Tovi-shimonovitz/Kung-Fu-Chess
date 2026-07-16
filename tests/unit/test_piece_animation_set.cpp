#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../include/graphics/piece_animation_set.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

namespace {

const std::string testRoot = "test_piece_animation_set_fixture";

void writeStateFolder(const std::string& stateName, const std::string& configContents) {
    const std::string stateFolder = testRoot + "/states/" + stateName;
    std::filesystem::create_directories(stateFolder + "/sprites");

    std::ofstream config(stateFolder + "/config.json");
    config << configContents;

    cv::Mat frame(4, 4, CV_8UC4, cv::Scalar(1, 2, 3, 255));
    cv::imwrite(stateFolder + "/sprites/1.png", frame);
}

void setupFixture() {
    std::filesystem::remove_all(testRoot);

    writeStateFolder("idle", R"({
  "physics": { "speed_m_per_sec": 0.0, "next_state_when_finished": "idle" },
  "graphics": { "frames_per_sec": 6, "is_loop": true }
})");

    writeStateFolder("move", R"({
  "physics": { "speed_m_per_sec": 1.5, "next_state_when_finished": "long_rest" },
  "graphics": { "frames_per_sec": 12, "is_loop": true }
})");

    writeStateFolder("jump", R"({
  "physics": { "speed_m_per_sec": 3.0, "next_state_when_finished": "short_rest" },
  "graphics": { "frames_per_sec": 8, "is_loop": false }
})");

    writeStateFolder("long_rest", R"({
  "physics": { "speed_m_per_sec": 0.0, "next_state_when_finished": "idle" },
  "graphics": { "frames_per_sec": 6, "is_loop": false }
})");

    writeStateFolder("short_rest", R"({
  "physics": { "speed_m_per_sec": 0.0, "next_state_when_finished": "idle" },
  "graphics": { "frames_per_sec": 8, "is_loop": false }
})");
}

}

void test_loads_all_five_states_correctly() {
    setupFixture();

    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);

    assert(set.clipFor("idle").config().framesPerSec == 6);
    assert(set.clipFor("idle").config().isLoop == true);

    assert(set.clipFor("move").config().framesPerSec == 12);
    assert(set.clipFor("move").config().nextStateWhenFinished == "long_rest");

    assert(set.clipFor("jump").config().nextStateWhenFinished == "short_rest");
    assert(set.clipFor("jump").config().isLoop == false);

    assert(set.clipFor("long_rest").config().framesPerSec == 6);
    assert(set.clipFor("short_rest").config().framesPerSec == 8);

    log_test("test_loads_all_five_states_correctly");
}

void test_unknown_state_throws() {
    setupFixture();

    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);

    bool threw = false;
    try {
        set.clipFor("no_such_state");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    log_test("test_unknown_state_throws");
}

int main() {
    std::cout << "--- Starting PieceAnimationSet Unit Tests ---" << std::endl;

    test_loads_all_five_states_correctly();
    test_unknown_state_throws();

    std::filesystem::remove_all(testRoot);

    std::cout << "--- All PieceAnimationSet tests passed successfully ---" << std::endl;
    return 0;
}
