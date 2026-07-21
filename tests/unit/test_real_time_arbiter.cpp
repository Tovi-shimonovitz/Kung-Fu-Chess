#include <iostream>
#include <cassert>
#include <memory>
#include "../../include/realtime/real_time_arbiter.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"
#include "../../include/engine/Result_structs.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_motion_stays_active_before_duration_elapses() {
    Board board(8, 8);
    board.addPiece({1, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {1, 0}));
    Piece* rook = board.getPieceAt({1, 0});
    RealTimeArbiter arbiter;

    arbiter.startMotion(rook, {1, 0}, {1, 2}); // 2 squares -> 2000ms
    assert(arbiter.hasActiveMotion() == true);

    arbiter.advanceTime(500, board);
    assert(arbiter.hasActiveMotion() == true);
    assert(board.getPieceAt({1, 0}) == rook);
    assert(board.getPieceAt({1, 2}) == nullptr);

    log_test("test_motion_stays_active_before_duration_elapses");
}

void test_motion_completes_and_moves_piece_on_arrival() {
    Board board(8, 8);
    board.addPiece({1, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {1, 0}));
    Piece* rook = board.getPieceAt({1, 0});
    RealTimeArbiter arbiter;

    arbiter.startMotion(rook, {1, 0}, {1, 2}); // 2 squares -> 2000ms
    WaitResult result = arbiter.advanceTime(2000, board);

    assert(arbiter.hasActiveMotion() == false);
    assert(board.getPieceAt({1, 0}) == nullptr);
    assert(board.getPieceAt({1, 2}) == rook);
    assert(rook->state == PieceState::COOLDOWN);   // ← במקום IDLE
    assert(result.was_processed == true);
    assert(result.message.find("motion_completed") != std::string::npos);

    arbiter.advanceTime(2000, board);   // משך המנוחה = משך התנועה עצמה
    assert(rook->state == PieceState::IDLE);   // ← עכשיו כן IDLE

    log_test("test_motion_completes_and_moves_piece_on_arrival");
}

void test_cooldown_piece_captured_does_not_crash() {
   
    Board board(8, 8);
    board.addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    board.addPiece({3, 2}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::ROOK, {3, 2}));
    Piece* whiteRook = board.getPieceAt({0, 0});
    Piece* blackRook = board.getPieceAt({3, 2});
    RealTimeArbiter arbiter;

    arbiter.startMotion(whiteRook, {0, 0}, {0, 2});   
    arbiter.startMotion(blackRook, {3, 2}, {0, 2});  

    arbiter.advanceTime(3000, board);
    assert(board.getPieceAt({0, 2}) == blackRook);  

    arbiter.advanceTime(2000, board);

    log_test("test_cooldown_piece_captured_does_not_crash");
}

void test_arrival_to_empty_square_does_not_crash() {
    
    Board board(8, 8);
    board.addPiece({3, 3}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::PAWN, {3, 3}));
    Piece* pawn = board.getPieceAt({3, 3});
    RealTimeArbiter arbiter;

    arbiter.startMotion(pawn, {3, 3}, {2, 3}); // forward one square onto an empty square
    WaitResult result = arbiter.advanceTime(1000, board);

    assert(result.message.find("king_eaten") == std::string::npos);
    assert(board.getPieceAt({2, 3}) == pawn);

    log_test("test_arrival_to_empty_square_does_not_crash");
}

void test_parallel_motions_finish_independently() {
    Board board(8, 8);
    board.addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    board.addPiece({7, 7}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KNIGHT, {7, 7}));
    Piece* rook = board.getPieceAt({0, 0});
    Piece* knight = board.getPieceAt({7, 7});
    RealTimeArbiter arbiter;

    arbiter.startMotion(rook, {0, 0}, {0, 1});   // 1 square -> 1000ms
    arbiter.startMotion(knight, {7, 7}, {5, 6}); // knight jump -> 2000ms (chebyshev distance 2)

    arbiter.advanceTime(1000, board);
    assert(board.getPieceAt({0, 1}) == rook);
    assert(arbiter.hasActiveMotion() == true);
    assert(board.getPieceAt({7, 7}) == knight);

    arbiter.advanceTime(1000, board);
    assert(arbiter.hasActiveMotion() == false);
    assert(board.getPieceAt({5, 6}) == knight);

    log_test("test_parallel_motions_finish_independently");
}

void test_king_capture_sets_king_eaten_message() {
    Board board(8, 8);
    board.addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    board.addPiece({0, 1}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::KING, {0, 1}));
    Piece* attacker = board.getPieceAt({0, 0});
    RealTimeArbiter arbiter;

    arbiter.startMotion(attacker, {0, 0}, {0, 1});
    WaitResult result = arbiter.advanceTime(1000, board);

    assert(result.message.find("king_eaten") != std::string::npos);
    assert(board.getPieceAt({0, 1}) == attacker);

    log_test("test_king_capture_sets_king_eaten_message");
}

void test_advance_time_in_arbitrary_chunks_matches_direct_call() {
   
    Board boardA(8, 8);
    boardA.addPiece({1, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {1, 0}));
    Piece* rookA = boardA.getPieceAt({1, 0});
    RealTimeArbiter arbiterA;
    arbiterA.startMotion(rookA, {1, 0}, {1, 1}); // 1000ms
    arbiterA.advanceTime(999, boardA);
    arbiterA.advanceTime(1, boardA);

    Board boardB(8, 8);
    boardB.addPiece({1, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {1, 0}));
    Piece* rookB = boardB.getPieceAt({1, 0});
    RealTimeArbiter arbiterB;
    arbiterB.startMotion(rookB, {1, 0}, {1, 1});
    arbiterB.advanceTime(1000, boardB);

    assert(arbiterA.hasActiveMotion() == arbiterB.hasActiveMotion());
    assert(boardA.getPieceAt({1, 1}) == rookA);
    assert(boardB.getPieceAt({1, 1}) == rookB);

    log_test("test_advance_time_in_arbitrary_chunks_matches_direct_call");
}

void test_bishop_motion_completes_diagonally() {
    Board board(8, 8);
    board.addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::BISHOP, {0, 0}));
    Piece* bishop = board.getPieceAt({0, 0});
    RealTimeArbiter arbiter;

    arbiter.startMotion(bishop, {0, 0}, {3, 3}); // 3 squares diagonally -> 3000ms
    WaitResult result = arbiter.advanceTime(3000, board);

    assert(arbiter.hasActiveMotion() == false);
    assert(board.getPieceAt({0, 0}) == nullptr);
    assert(board.getPieceAt({3, 3}) == bishop);
    assert(bishop->state == PieceState::COOLDOWN);
    assert(result.was_processed == true);

    log_test("test_bishop_motion_completes_diagonally");
}

void test_queen_motion_completes_diagonally() {
    Board board(8, 8);
    board.addPiece({3, 3}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::QUEEN, {3, 3}));
    Piece* queen = board.getPieceAt({3, 3});
    RealTimeArbiter arbiter;

    arbiter.startMotion(queen, {3, 3}, {6, 6}); // 3 squares diagonally -> 3000ms
    WaitResult result = arbiter.advanceTime(3000, board);

    assert(arbiter.hasActiveMotion() == false);
    assert(board.getPieceAt({3, 3}) == nullptr);
    assert(board.getPieceAt({6, 6}) == queen);
    assert(queen->state == PieceState::COOLDOWN);
    assert(result.was_processed == true);

    log_test("test_queen_motion_completes_diagonally");
}

void test_king_motion_completes_one_square() {
    Board board(8, 8);
    board.addPiece({3, 3}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::KING, {3, 3}));
    Piece* king = board.getPieceAt({3, 3});
    RealTimeArbiter arbiter;

    arbiter.startMotion(king, {3, 3}, {4, 4}); // 1 square -> 1000ms
    WaitResult result = arbiter.advanceTime(1000, board);

    assert(arbiter.hasActiveMotion() == false);
    assert(board.getPieceAt({3, 3}) == nullptr);
    assert(board.getPieceAt({4, 4}) == king);
    assert(king->state == PieceState::COOLDOWN);
    assert(result.was_processed == true);

    log_test("test_king_motion_completes_one_square");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING REAL-TIME ARBITER UNIT TESTS " << std::endl;
    std::cout << "========================================" << std::endl;

    test_motion_stays_active_before_duration_elapses();
    test_motion_completes_and_moves_piece_on_arrival();
    test_cooldown_piece_captured_does_not_crash();
    test_arrival_to_empty_square_does_not_crash();
    test_parallel_motions_finish_independently();
    test_king_capture_sets_king_eaten_message();
    test_advance_time_in_arbitrary_chunks_matches_direct_call();
    test_bishop_motion_completes_diagonally();
    test_queen_motion_completes_diagonally();
    test_king_motion_completes_one_square();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL TESTS EXECUTED                    " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
