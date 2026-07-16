#include <iostream>
#include <cassert>
#include <memory>
#include "../../include/realtime/standard_collision_resolver.h"
#include "../../include/realtime/real_time_arbiter.h"
#include "../../include/model/Board.h"
#include "../../include/model/PieceFactory.h"
#include "../../include/model/Piece_enums.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

Motion makeMotionWithElapsed(Piece* piece, Position src, Position dst, int elapsedMs) {
    int duration = Motion::calculateDuration(src, dst);
    return Motion(piece, src, dst, duration - elapsedMs);
}

// ---------------------------------------------------------------------------
// StandardCollisionResolver: unit tests (exact Motion states, no simulation)
// ---------------------------------------------------------------------------

void test_resolver_rivals_later_arrival_eats_earlier() {
    Piece white(PieceColor::WHITE, PieceKind::ROOK);
    Piece black(PieceColor::BLACK, PieceKind::ROOK);
    Motion earlyArrival = makeMotionWithElapsed(&white, Position(0, 0), Position(0, 10), 1200); // penetration 700
    Motion lateArrival = makeMotionWithElapsed(&black, Position(0, 0), Position(0, 10), 1700);  // penetration 200

    StandardCollisionResolver resolver;
    CollisionDecision decision = resolver.resolve(earlyArrival, lateArrival, 50);

    assert(decision.outcome == CollisionOutcome::CAPTURE);
    assert(decision.earlier == &earlyArrival);
    assert(decision.later == &lateArrival);
    log_test("test_resolver_rivals_later_arrival_eats_earlier");
}

void test_resolver_rivals_exact_tie_defaults_to_first_argument() {
    Piece white(PieceColor::WHITE, PieceKind::ROOK);
    Piece black(PieceColor::BLACK, PieceKind::ROOK);
    Motion a = makeMotionWithElapsed(&white, Position(0, 0), Position(0, 10), 3500);
    Motion b = makeMotionWithElapsed(&black, Position(0, 0), Position(0, 10), 3500);

    StandardCollisionResolver resolver;
    CollisionDecision decision = resolver.resolve(a, b, 50);

    assert(decision.outcome == CollisionOutcome::CAPTURE);
    assert(decision.earlier == &a);
    assert(decision.later == &b);
    log_test("test_resolver_rivals_exact_tie_defaults_to_first_argument");
}

void test_resolver_same_color_resident_blocks_newcomer() {
    Piece white1(PieceColor::WHITE, PieceKind::ROOK);
    Piece white2(PieceColor::WHITE, PieceKind::ROOK);
    // Resident: has occupied the shared square (4,4) since elapsed 3500; now at 4000.
    Motion resident = makeMotionWithElapsed(&white1, Position(4, 0), Position(4, 8), 4000);
    // Newcomer: crosses into the shared square (4,4) only during this very tick.
    Motion newcomer = makeMotionWithElapsed(&white2, Position(0, 4), Position(8, 4), 3520);

    StandardCollisionResolver resolver;
    CollisionDecision decision = resolver.resolve(resident, newcomer, 50);

    assert(decision.outcome == CollisionOutcome::BLOCK);
    assert(decision.earlier == &resident);
    assert(decision.later == &newcomer);
    log_test("test_resolver_same_color_resident_blocks_newcomer");
}

void test_resolver_same_color_arrived_together_stops_whoever_set_out_earlier() {
    Piece white1(PieceColor::WHITE, PieceKind::QUEEN);
    Piece white2(PieceColor::WHITE, PieceKind::QUEEN);
    // Both cross into the shared square (4,4) on the exact same tick, but P set out
    // recently (elapsed 3520) while Q has been travelling far longer (elapsed 23520).
    Motion p = makeMotionWithElapsed(&white1, Position(4, 0), Position(4, 8), 3520);
    Motion q = makeMotionWithElapsed(&white2, Position(4, -20), Position(4, 20), 23520);

    StandardCollisionResolver resolver;
    CollisionDecision decision = resolver.resolve(p, q, 50);

    assert(decision.outcome == CollisionOutcome::BLOCK);
    assert(decision.earlier == &q); // set out earlier -> stops
    assert(decision.later == &p);   // set out more recently -> continues
    log_test("test_resolver_same_color_arrived_together_stops_whoever_set_out_earlier");
}

void test_resolver_against_stationary_rival_is_capture() {
    Piece mover(PieceColor::WHITE, PieceKind::ROOK);
    Piece stationary(PieceColor::BLACK, PieceKind::PAWN);
    Motion motion = makeMotionWithElapsed(&mover, Position(0, 0), Position(0, 5), 2000);

    StandardCollisionResolver resolver;
    CollisionOutcome outcome = resolver.resolveAgainstStationary(motion, stationary);

    assert(outcome == CollisionOutcome::CAPTURE);
    log_test("test_resolver_against_stationary_rival_is_capture");
}

void test_resolver_against_stationary_friendly_is_block() {
    Piece mover(PieceColor::WHITE, PieceKind::ROOK);
    Piece stationary(PieceColor::WHITE, PieceKind::PAWN);
    Motion motion = makeMotionWithElapsed(&mover, Position(0, 0), Position(0, 5), 2000);

    StandardCollisionResolver resolver;
    CollisionOutcome outcome = resolver.resolveAgainstStationary(motion, stationary);

    assert(outcome == CollisionOutcome::BLOCK);
    log_test("test_resolver_against_stationary_friendly_is_block");
}

// ---------------------------------------------------------------------------
// RealTimeArbiter integration tests: full detect+apply pipeline on a real Board
// ---------------------------------------------------------------------------

void test_integration_moving_piece_captures_stationary_rival_mid_path() {
    Board board(8, 1);
    board.addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    board.addPiece({0, 4}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::PAWN, {0, 4}));
    Piece* rook = board.getPieceAt({0, 0});

    RealTimeArbiter arbiter;
    arbiter.startMotion(rook, {0, 0}, {0, 7}); // would normally travel on to (0,7)
    arbiter.advanceTime(4000, board);

    assert(board.getPieceAt({0, 4}) == rook);
    assert(board.getPieceAt({0, 0}) == nullptr);
    assert(board.getPieceAt({0, 7}) == nullptr);
    assert(arbiter.hasActiveMotion() == false);
    log_test("test_integration_moving_piece_captures_stationary_rival_mid_path");
}

void test_integration_moving_piece_blocked_by_stationary_friendly_mid_path() {
    Board board(8, 1);
    board.addPiece({0, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 0}));
    board.addPiece({0, 4}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::PAWN, {0, 4}));
    Piece* rook = board.getPieceAt({0, 0});
    Piece* friendlyPawn = board.getPieceAt({0, 4});

    RealTimeArbiter arbiter;
    arbiter.startMotion(rook, {0, 0}, {0, 7});
    arbiter.advanceTime(4000, board);

    assert(board.getPieceAt({0, 3}) == rook);
    assert(board.getPieceAt({0, 4}) == friendlyPawn);
    assert(board.getPieceAt({0, 0}) == nullptr);
    assert(arbiter.hasActiveMotion() == false);
    log_test("test_integration_moving_piece_blocked_by_stationary_friendly_mid_path");
}

void test_integration_two_rival_motions_meeting_simultaneously() {
    Board board(9, 9);
    board.addPiece({0, 4}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 4}));
    board.addPiece({4, 0}, PieceFactory::createPiece(PieceColor::BLACK, PieceKind::ROOK, {4, 0}));
    Piece* vertical = board.getPieceAt({0, 4});
    Piece* horizontal = board.getPieceAt({4, 0});

    RealTimeArbiter arbiter;
    arbiter.startMotion(vertical, {0, 4}, {8, 4});
    arbiter.startMotion(horizontal, {4, 0}, {4, 8});
    arbiter.advanceTime(3500, board); // both cross into (4,4) on the same tick

    // The first-passed motion ("earlier") is captured; the second ("later") survives
    // and its move ends at the collision square rather than its original destination.
    assert(board.getPieceAt({4, 4}) == horizontal);
    assert(board.getPieceAt({0, 4}) == nullptr);
    assert(board.getPieceAt({4, 0}) == nullptr);
    assert(arbiter.hasActiveMotion() == false);
    log_test("test_integration_two_rival_motions_meeting_simultaneously");
}

void test_integration_two_friendly_motions_meeting_simultaneously() {
    Board board(9, 9);
    board.addPiece({0, 4}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::ROOK, {0, 4}));
    board.addPiece({4, 0}, PieceFactory::createPiece(PieceColor::WHITE, PieceKind::QUEEN, {4, 0}));
    Piece* vertical = board.getPieceAt({0, 4});
    Piece* horizontal = board.getPieceAt({4, 0});

    RealTimeArbiter arbiter;
    arbiter.startMotion(vertical, {0, 4}, {8, 4});
    arbiter.startMotion(horizontal, {4, 0}, {4, 8});
    arbiter.advanceTime(3500, board);

    // Both pieces survive: the "later" one (per the resolver's tie-break) stops one
    // square short instead of entering the shared square; the other continues flying.
    assert(board.getPieceAt({4, 3}) == horizontal);
    assert(board.getPieceAt({4, 0}) == nullptr);
    assert(board.getPieceAt({0, 4}) == vertical); // still mid-flight, Board still shows its source
    assert(arbiter.hasActiveMotion() == true);
    log_test("test_integration_two_friendly_motions_meeting_simultaneously");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING COLLISION UNIT TESTS         " << std::endl;
    std::cout << "========================================" << std::endl;

    test_resolver_rivals_later_arrival_eats_earlier();
    test_resolver_rivals_exact_tie_defaults_to_first_argument();
    test_resolver_same_color_resident_blocks_newcomer();
    test_resolver_same_color_arrived_together_stops_whoever_set_out_earlier();
    test_resolver_against_stationary_rival_is_capture();
    test_resolver_against_stationary_friendly_is_block();

    test_integration_moving_piece_captures_stationary_rival_mid_path();
    test_integration_moving_piece_blocked_by_stationary_friendly_mid_path();
    test_integration_two_rival_motions_meeting_simultaneously();
    test_integration_two_friendly_motions_meeting_simultaneously();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL COLLISION TESTS EXECUTED          " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
