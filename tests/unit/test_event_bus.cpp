#include <cassert>
#include <iostream>
#include "../../include/engine/EventBus.h"
#include "../../include/engine/GameEvents.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_subscribe_and_publish_delivers_correct_data() {
    EventBus bus;
    bool called = false;
    Position receivedSource;
    Position receivedTarget;
    Piece* receivedPiece = nullptr;

    bus.subscribe<MoveEvent>([&](const MoveEvent& event) {
        called = true;
        receivedSource = event.source;
        receivedTarget = event.target;
        receivedPiece = event.piece;
    });

    Piece* dummyPiece = reinterpret_cast<Piece*>(0x1);
    bus.publish(MoveEvent{Position(1, 2), Position(3, 4), dummyPiece});

    assert(called);
    assert(receivedSource == Position(1, 2));
    assert(receivedTarget == Position(3, 4));
    assert(receivedPiece == dummyPiece);
    log_test("test_subscribe_and_publish_delivers_correct_data");
}

void test_multiple_handlers_for_same_event_all_called() {
    EventBus bus;
    int callCount = 0;

    bus.subscribe<GameStartedEvent>([&](const GameStartedEvent&) { ++callCount; });
    bus.subscribe<GameStartedEvent>([&](const GameStartedEvent&) { ++callCount; });

    bus.publish(GameStartedEvent{});

    assert(callCount == 2);
    log_test("test_multiple_handlers_for_same_event_all_called");
}

void test_publish_with_no_subscribers_does_not_crash() {
    EventBus bus;
    bus.publish(PieceCapturedEvent{PieceKind::PAWN, PieceColor::BLACK, PieceColor::WHITE});
    log_test("test_publish_with_no_subscribers_does_not_crash");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING EVENT BUS UNIT TESTS          " << std::endl;
    std::cout << "========================================" << std::endl;

    test_subscribe_and_publish_delivers_correct_data();
    test_multiple_handlers_for_same_event_all_called();
    test_publish_with_no_subscribers_does_not_crash();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL EVENT BUS TESTS EXECUTED           " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
