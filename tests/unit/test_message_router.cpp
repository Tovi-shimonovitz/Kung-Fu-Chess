#include <cassert>
#include <iostream>
#include "../../include/network/message_router.h"
#include "../../include/network/message_codec.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_register_allowed_when_connected() {
    ConnectionsRegistry registry;
    registry.add(1);
    MessageRouter router(registry);

    RawMessage raw = MessageCodec::toRaw(RegisterMessage{"alice"});
    auto error = router.route(1, raw);

    assert(!error.has_value());
    auto info = registry.get(1);
    assert(info->status == ConnectionStatus::Registered);
    assert(info->username == "alice");
    log_test("test_register_allowed_when_connected");
}

void test_move_request_rejected_when_not_in_game() {
    ConnectionsRegistry registry;
    registry.add(1);
    MessageRouter router(registry);

    RawMessage raw = MessageCodec::toRaw(MoveRequestMessage{Position(0, 0), Position(1, 1)});
    auto error = router.route(1, raw);

    assert(error.has_value());
    log_test("test_move_request_rejected_when_not_in_game");
}

void test_play_request_rejected_before_register() {
    ConnectionsRegistry registry;
    registry.add(1);
    MessageRouter router(registry);

    RawMessage raw = MessageCodec::toRaw(PlayRequestMessage{});
    auto error = router.route(1, raw);

    assert(error.has_value());
    log_test("test_play_request_rejected_before_register");
}

void test_play_request_allowed_after_register() {
    ConnectionsRegistry registry;
    registry.add(1);
    MessageRouter router(registry);
    router.route(1, MessageCodec::toRaw(RegisterMessage{"bob"}));

    auto error = router.route(1, MessageCodec::toRaw(PlayRequestMessage{}));

    assert(!error.has_value());
    log_test("test_play_request_allowed_after_register");
}

void test_route_on_unknown_connection_is_ignored() {
    ConnectionsRegistry registry;
    MessageRouter router(registry);

    auto error = router.route(999, MessageCodec::toRaw(RegisterMessage{"ghost"}));

    assert(!error.has_value());
    log_test("test_route_on_unknown_connection_is_ignored");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING MESSAGE ROUTER UNIT TESTS     " << std::endl;
    std::cout << "========================================" << std::endl;

    test_register_allowed_when_connected();
    test_move_request_rejected_when_not_in_game();
    test_play_request_rejected_before_register();
    test_play_request_allowed_after_register();
    test_route_on_unknown_connection_is_ignored();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL MESSAGE ROUTER TESTS EXECUTED      " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
