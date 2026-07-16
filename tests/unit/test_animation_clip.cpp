#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../include/graphics/animation_clip.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

namespace {

const std::string testRoot = "test_animation_clip_fixture";

void writeConfig(const std::string& folder, const std::string& contents) {
    std::ofstream file(folder + "/config.json");
    file << contents;
}

void writeFrame(const std::string& spritesFolder, int frameNumber) {
    cv::Mat frame(4, 4, CV_8UC4, cv::Scalar(10, 20, 30, 255));
    cv::imwrite(spritesFolder + "/" + std::to_string(frameNumber) + ".png", frame);
}

void setupFixtureWithFrames(int frameCount) {
    std::filesystem::remove_all(testRoot);
    std::filesystem::create_directories(testRoot + "/sprites");

    writeConfig(testRoot, R"({
  "physics": { "speed_m_per_sec": 1.5, "next_state_when_finished": "idle" },
  "graphics": { "frames_per_sec": 10, "is_loop": false }
})");

    for (int frameNumber = 1; frameNumber <= frameCount; ++frameNumber) {
        writeFrame(testRoot + "/sprites", frameNumber);
    }
}

}

void test_loads_config_and_all_frames() {
    setupFixtureWithFrames(3);

    AnimationClip clip = AnimationClip::loadFromFolder(testRoot);

    assert(clip.frameCount() == 3);
    assert(clip.config().framesPerSec == 10);
    assert(clip.config().nextStateWhenFinished == "idle");
    assert(clip.config().isLoop == false);
    assert(clip.frameAt(0).is_loaded());
    assert(clip.frameAt(2).is_loaded());

    log_test("test_loads_config_and_all_frames");
}

void test_frame_at_out_of_range_throws() {
    setupFixtureWithFrames(2);

    AnimationClip clip = AnimationClip::loadFromFolder(testRoot);

    bool threw = false;
    try {
        clip.frameAt(2); // valid indices are only 0 and 1
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);

    log_test("test_frame_at_out_of_range_throws");
}

void test_missing_frames_throws() {
    std::filesystem::remove_all(testRoot);
    std::filesystem::create_directories(testRoot + "/sprites");
    writeConfig(testRoot, R"({
  "physics": { "speed_m_per_sec": 0.0, "next_state_when_finished": "idle" },
  "graphics": { "frames_per_sec": 6, "is_loop": true }
})");
    // no sprite files written

    bool threw = false;
    try {
        AnimationClip::loadFromFolder(testRoot);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);

    log_test("test_missing_frames_throws");
}

int main() {
    std::cout << "--- Starting AnimationClip Unit Tests ---" << std::endl;

    test_loads_config_and_all_frames();
    test_frame_at_out_of_range_throws();
    test_missing_frames_throws();

    std::filesystem::remove_all(testRoot);

    std::cout << "--- All AnimationClip tests passed successfully ---" << std::endl;
    return 0;
}
