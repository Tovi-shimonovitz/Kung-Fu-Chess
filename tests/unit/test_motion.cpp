#include <iostream>
#include <cassert>
#include <cmath>
#include "../../include/realtime/motion.h"
#include "../../include/model/Piece.h"
#include "../../include/model/Piece_enums.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

bool nearlyEqual(double a, double b) {
    return std::abs(a - b) < 1e-9;
}

void test_calculate_duration_straight_line() {
    assert(Motion::calculateDuration(Position(0, 0), Position(0, 5)) == 5000);
    assert(Motion::calculateDuration(Position(0, 0), Position(5, 0)) == 5000);
    log_test("test_calculate_duration_straight_line");
}

void test_calculate_duration_diagonal() {
    assert(Motion::calculateDuration(Position(2, 2), Position(6, 6)) == 4000);
    log_test("test_calculate_duration_diagonal");
}

void test_calculate_duration_is_direction_agnostic() {
    assert(Motion::calculateDuration(Position(5, 5), Position(2, 5)) == 3000);
    assert(Motion::calculateDuration(Position(2, 5), Position(5, 5)) == 3000);
    log_test("test_calculate_duration_is_direction_agnostic");
}

void test_elapsed_ms_derived_from_remaining_time() {
    Piece piece(PieceColor::WHITE, PieceKind::ROOK);
    Motion motion(&piece, Position(0, 0), Position(0, 4), Motion::calculateDuration(Position(0, 0), Position(0, 4)));

    assert(motion.totalDurationMs() == 4000);
    assert(motion.elapsedMs() == 0);

    motion.remainingTimeMs = 1000;
    assert(motion.elapsedMs() == 3000);

    log_test("test_elapsed_ms_derived_from_remaining_time");
}

void test_position_at_elapsed_forward_direction() {
    Piece piece(PieceColor::WHITE, PieceKind::ROOK);
    Motion motion(&piece, Position(5, 0), Position(2, 0), Motion::calculateDuration(Position(5, 0), Position(2, 0)));

    ExactPosition halfway = motion.positionAtElapsed(1500); // half of 3000ms
    assert(nearlyEqual(halfway.row, 3.5));
    assert(nearlyEqual(halfway.col, 0.0));

    log_test("test_position_at_elapsed_forward_direction");
}

void test_position_at_elapsed_handles_decreasing_coordinates() {
    // Target row (2) is smaller than source row (5): motion moving "backwards".
    Piece piece(PieceColor::WHITE, PieceKind::ROOK);
    Motion motion(&piece, Position(5, 0), Position(2, 0), Motion::calculateDuration(Position(5, 0), Position(2, 0)));

    ExactPosition atStart = motion.positionAtElapsed(0);
    assert(nearlyEqual(atStart.row, 5.0));

    ExactPosition atEnd = motion.positionAtElapsed(3000);
    assert(nearlyEqual(atEnd.row, 2.0));

    log_test("test_position_at_elapsed_handles_decreasing_coordinates");
}

void test_current_position_matches_position_at_elapsed() {
    Piece piece(PieceColor::WHITE, PieceKind::ROOK);
    int duration = Motion::calculateDuration(Position(0, 0), Position(0, 4));
    Motion motion(&piece, Position(0, 0), Position(0, 4), duration);
    motion.remainingTimeMs = duration - 1000; // 1000ms elapsed

    ExactPosition current = motion.currentPosition();
    assert(nearlyEqual(current.col, 1.0));
    assert(nearlyEqual(current.row, 0.0));

    log_test("test_current_position_matches_position_at_elapsed");
}

void test_is_finished_when_remaining_time_reaches_zero_or_below() {
    Piece piece(PieceColor::WHITE, PieceKind::PAWN);
    Motion motion(&piece, Position(0, 0), Position(0, 1), 1000);

    assert(motion.isFinished() == false);
    motion.remainingTimeMs = 0;
    assert(motion.isFinished() == true);
    motion.remainingTimeMs = -50;
    assert(motion.isFinished() == true);

    log_test("test_is_finished_when_remaining_time_reaches_zero_or_below");
}

void test_round_to_square_rounds_to_nearest_integer() {
    assert(roundToSquare(ExactPosition{4.99, 5.001}) == Position(5, 5));
    assert(roundToSquare(ExactPosition{4.49, 4.51}) == Position(4, 5));
    log_test("test_round_to_square_rounds_to_nearest_integer");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING MOTION UNIT TESTS            " << std::endl;
    std::cout << "========================================" << std::endl;

    test_calculate_duration_straight_line();
    test_calculate_duration_diagonal();
    test_calculate_duration_is_direction_agnostic();
    test_elapsed_ms_derived_from_remaining_time();
    test_position_at_elapsed_forward_direction();
    test_position_at_elapsed_handles_decreasing_coordinates();
    test_current_position_matches_position_at_elapsed();
    test_is_finished_when_remaining_time_reaches_zero_or_below();
    test_round_to_square_rounds_to_nearest_integer();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL MOTION TESTS EXECUTED             " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
