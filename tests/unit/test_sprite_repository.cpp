#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "../../include/graphics/sprites/sprite_repository.h"
#include "../../include/graphics/sprites/piece_code.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

namespace {

const std::string testRoot = "test_sprite_repository_fixture";

void writeStateFolder(const std::string& pieceFolder, const std::string& stateName) {
    const std::string stateFolder = pieceFolder + "/states/" + stateName;
    std::filesystem::create_directories(stateFolder + "/sprites");

    std::ofstream config(stateFolder + "/config.json");
    config << R"({
  "physics": { "speed_m_per_sec": 0.0, "next_state_when_finished": "idle" },
  "graphics": { "frames_per_sec": 6, "is_loop": true }
})";

    cv::Mat frame(4, 4, CV_8UC4, cv::Scalar(1, 2, 3, 255));
    cv::imwrite(stateFolder + "/sprites/1.png", frame);
}

void writePieceFolder(const std::string& pieceFolder) {
    static const std::vector<std::string> states = { "idle", "move", "jump", "long_rest", "short_rest" };
    for (const std::string& state : states) {
        writeStateFolder(pieceFolder, state);
    }
}

void setupFixtureForOnePiece(PieceColor color, PieceKind kind) {
    std::filesystem::remove_all(testRoot);
    const std::string code = PieceCode::shortCode(color, kind);
    writePieceFolder(testRoot + "/" + code);
}

void setupFixtureForAllPieces() {
    std::filesystem::remove_all(testRoot);

    static const std::vector<PieceColor> allColors = { PieceColor::WHITE, PieceColor::BLACK };
    static const std::vector<PieceKind> allKinds = {
        PieceKind::KING, PieceKind::QUEEN, PieceKind::ROOK,
        PieceKind::BISHOP, PieceKind::KNIGHT, PieceKind::PAWN
    };

    for (PieceColor color : allColors) {
        for (PieceKind kind : allKinds) {
            const std::string code = PieceCode::shortCode(color, kind);
            writePieceFolder(testRoot + "/" + code);
        }
    }
}

}

void test_get_returns_same_cached_instance() {
    setupFixtureForOnePiece(PieceColor::WHITE, PieceKind::KING);

    SpriteRepository repository(testRoot);

    PieceAnimationSet& first = repository.get(PieceColor::WHITE, PieceKind::KING);
    PieceAnimationSet& second = repository.get(PieceColor::WHITE, PieceKind::KING);

    assert(&first == &second);

    log_test("test_get_returns_same_cached_instance");
}

void test_preload_all_survives_even_if_disk_is_removed_afterward() {
    setupFixtureForAllPieces();

    SpriteRepository repository(testRoot);
    repository.preloadAll();

    std::filesystem::remove_all(testRoot); // the disk copy is gone now

    // if get() still needed the disk at this point, this would throw
    PieceAnimationSet& queen = repository.get(PieceColor::BLACK, PieceKind::QUEEN);
    assert(queen.clipFor("idle").frameCount() == 1);

    log_test("test_preload_all_survives_even_if_disk_is_removed_afterward");
}

int main() {
    std::cout << "--- Starting SpriteRepository Unit Tests ---" << std::endl;

    test_get_returns_same_cached_instance();
    test_preload_all_survives_even_if_disk_is_removed_afterward();

    std::cout << "--- All SpriteRepository tests passed successfully ---" << std::endl;
    return 0;
}
