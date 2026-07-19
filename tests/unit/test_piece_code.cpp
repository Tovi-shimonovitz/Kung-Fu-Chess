#include <cassert>
#include <iostream>
#include "../../include/graphics/sprites/piece_code.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_short_code_white_pieces() {
    assert(PieceCode::shortCode(PieceColor::WHITE, PieceKind::KING) == "KW");
    assert(PieceCode::shortCode(PieceColor::WHITE, PieceKind::QUEEN) == "QW");
    assert(PieceCode::shortCode(PieceColor::WHITE, PieceKind::KNIGHT) == "NW");
    log_test("test_short_code_white_pieces");
}

void test_short_code_black_pieces() {
    assert(PieceCode::shortCode(PieceColor::BLACK, PieceKind::ROOK) == "RB");
    assert(PieceCode::shortCode(PieceColor::BLACK, PieceKind::BISHOP) == "BB");
    assert(PieceCode::shortCode(PieceColor::BLACK, PieceKind::PAWN) == "PB");
    log_test("test_short_code_black_pieces");
}

void test_short_code_rejects_missing_color_or_kind() {
    bool threw = false;
    try {
        PieceCode::shortCode(PieceColor::None, PieceKind::PAWN);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        PieceCode::shortCode(PieceColor::WHITE, PieceKind::None);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    log_test("test_short_code_rejects_missing_color_or_kind");
}

void test_state_folder_name_mapping() {
    assert(PieceCode::stateFolderName(PieceState::IDLE) == "idle");
    assert(PieceCode::stateFolderName(PieceState::MOVING) == "move");
    assert(PieceCode::stateFolderName(PieceState::COOLDOWN) == "long_rest");
    log_test("test_state_folder_name_mapping");
}

void test_state_folder_name_rejects_captured() {
    bool threw = false;
    try {
        PieceCode::stateFolderName(PieceState::CAPTURED);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    log_test("test_state_folder_name_rejects_captured");
}

int main() {
    std::cout << "--- Starting PieceCode Unit Tests ---" << std::endl;

    test_short_code_white_pieces();
    test_short_code_black_pieces();
    test_short_code_rejects_missing_color_or_kind();
    test_state_folder_name_mapping();
    test_state_folder_name_rejects_captured();

    std::cout << "--- All PieceCode tests passed successfully ---" << std::endl;
    return 0;
}
