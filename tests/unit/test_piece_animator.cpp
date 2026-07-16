#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../include/graphics/piece_animator.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

namespace {

const std::string testRoot = "test_piece_animator_fixture";

void writeStateFolder(const std::string& stateName, bool isLoop, int frameCount) {
    const std::string stateFolder = testRoot + "/states/" + stateName;
    std::filesystem::create_directories(stateFolder + "/sprites");

    std::ofstream config(stateFolder + "/config.json");
    config << "{\n"
              "  \"physics\": { \"speed_m_per_sec\": 0.0, \"next_state_when_finished\": \"idle\" },\n"
              "  \"graphics\": { \"frames_per_sec\": 10, \"is_loop\": " << (isLoop ? "true" : "false") << " }\n"
              "}";

    for (int frameNumber = 1; frameNumber <= frameCount; ++frameNumber) {
        cv::Mat frame(4, 4, CV_8UC4, cv::Scalar(frameNumber, frameNumber, frameNumber, 255));
        cv::imwrite(stateFolder + "/sprites/" + std::to_string(frameNumber) + ".png", frame);
    }
}

void setupFixture() {
    std::filesystem::remove_all(testRoot);

    writeStateFolder("idle", true, 2);
    writeStateFolder("move", true, 2);
    writeStateFolder("jump", false, 2);
    writeStateFolder("long_rest", false, 2);
    writeStateFolder("short_rest", false, 2);
}

}

void test_initial_state_is_idle() {
    setupFixture();
    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);
    PieceAnimator animator(set);

    assert(animator.stateName() == "idle");
    assert(animator.frameIndex() == 0);

    log_test("test_initial_state_is_idle");
}

void test_sync_to_moving_switches_immediately() {
    setupFixture();
    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);
    PieceAnimator animator(set);

    animator.syncToGameState(PieceState::MOVING);

    assert(animator.stateName() == "move");
    assert(animator.frameIndex() == 0);

    log_test("test_sync_to_moving_switches_immediately");
}

void test_sync_to_same_state_does_not_reset_progress() {
    setupFixture();
    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);
    PieceAnimator animator(set);

    animator.syncToGameState(PieceState::MOVING);
    animator.advance(100); // frames_per_sec=10 -> 100ms per frame -> should move to frame 1
    assert(animator.frameIndex() == 1);

    animator.syncToGameState(PieceState::MOVING); // still MOVING, nothing should change
    assert(animator.frameIndex() == 1);

    log_test("test_sync_to_same_state_does_not_reset_progress");
}

void test_advance_loops_for_looping_clip() {
    setupFixture();
    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);
    PieceAnimator animator(set); // starts in "idle", is_loop=true, 2 frames @ 10fps

    animator.advance(100);
    assert(animator.frameIndex() == 1);

    animator.advance(100); // ran off the end of a looping clip -> wraps back to 0
    assert(animator.frameIndex() == 0);

    log_test("test_advance_loops_for_looping_clip");
}

void test_advance_freezes_on_last_frame_for_non_looping_clip() {
    setupFixture();
    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);
    PieceAnimator animator(set);

    animator.syncToGameState(PieceState::COOLDOWN); // maps to "long_rest", is_loop=false, 2 frames

    animator.advance(100);
    assert(animator.frameIndex() == 1);

    animator.advance(1000); // way past the clip's total length
    assert(animator.frameIndex() == 1); // frozen on the last frame, not out of range

    log_test("test_advance_freezes_on_last_frame_for_non_looping_clip");
}

void test_current_frame_returns_a_loaded_image() {
    setupFixture();
    PieceAnimationSet set = PieceAnimationSet::loadFromFolder(testRoot);
    PieceAnimator animator(set);

    assert(animator.currentFrame().is_loaded());

    log_test("test_current_frame_returns_a_loaded_image");
}

int main() {
    std::cout << "--- Starting PieceAnimator Unit Tests ---" << std::endl;

    test_initial_state_is_idle();
    test_sync_to_moving_switches_immediately();
    test_sync_to_same_state_does_not_reset_progress();
    test_advance_loops_for_looping_clip();
    test_advance_freezes_on_last_frame_for_non_looping_clip();
    test_current_frame_returns_a_loaded_image();

    std::filesystem::remove_all(testRoot);

    std::cout << "--- All PieceAnimator tests passed successfully ---" << std::endl;
    return 0;
}
