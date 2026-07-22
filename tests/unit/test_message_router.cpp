#include <cassert>
#include <cstdio>
#include <iostream>
#include "../../include/network/core/message_router.h"
#include "../../include/network/protocol/message_codec.h"
#include "../../include/network/core/matchmaker.h"
#include "../../include/network/registry/games_registry.h"
#include "../../include/network/core/game_server.h"
#include "../../include/db/user_repository.h"
#include "../../include/network/core/auth_handler.h"
#include "../../include/io/env_config.h"

namespace {
std::string boardCsvPath() {
    return EnvConfig::load(".env").get("BOARD_CSV_PATH");
}

const char* TEST_DB_PATH = "test_message_router.db";
}

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_register_allowed_when_connected() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games;
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    MessageRouter router(registry, matchmaker, games, authHandler);

    RawMessage raw = MessageCodec::toRaw(RegisterMessage{"alice", "pw"});
    auto error = router.route(1, raw);

    assert(!error.has_value());
    auto info = registry.get(1);
    assert(info->status == ConnectionStatus::Registered);
    assert(info->username == "alice");
    std::remove(TEST_DB_PATH);
    log_test("test_register_allowed_when_connected");
}

void test_register_rejects_wrong_password() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    registry.add(2);
    GamesRegistry games;
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    MessageRouter router(registry, matchmaker, games, authHandler);

    router.route(1, MessageCodec::toRaw(RegisterMessage{"alice", "correct_pw"}));
    auto error = router.route(2, MessageCodec::toRaw(RegisterMessage{"alice", "wrong_pw"}));

    assert(error.has_value());
    auto info = registry.get(2);
    assert(info->status == ConnectionStatus::Connected);
    std::remove(TEST_DB_PATH);
    log_test("test_register_rejects_wrong_password");
}

void test_move_request_rejected_when_not_in_game() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games;
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    MessageRouter router(registry, matchmaker, games, authHandler);

    RawMessage raw = MessageCodec::toRaw(MoveRequestMessage{Position(0, 0), Position(1, 1)});
    auto error = router.route(1, raw);

    assert(error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_move_request_rejected_when_not_in_game");
}

void test_play_request_rejected_before_register() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games;
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    MessageRouter router(registry, matchmaker, games, authHandler);

    RawMessage raw = MessageCodec::toRaw(PlayRequestMessage{});
    auto error = router.route(1, raw);

    assert(error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_play_request_rejected_before_register");
}

void test_play_request_allowed_after_register() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games;
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    MessageRouter router(registry, matchmaker, games, authHandler);
    router.route(1, MessageCodec::toRaw(RegisterMessage{"bob", "pw"}));

    auto error = router.route(1, MessageCodec::toRaw(PlayRequestMessage{}));

    assert(!error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_play_request_allowed_after_register");
}

void test_route_on_unknown_connection_is_ignored() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    GamesRegistry games;
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    MessageRouter router(registry, matchmaker, games, authHandler);

    auto error = router.route(999, MessageCodec::toRaw(RegisterMessage{"ghost", "pw"}));

    assert(!error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_route_on_unknown_connection_is_ignored");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING MESSAGE ROUTER UNIT TESTS     " << std::endl;
    std::cout << "========================================" << std::endl;

    test_register_allowed_when_connected();
    test_register_rejects_wrong_password();
    test_move_request_rejected_when_not_in_game();
    test_play_request_rejected_before_register();
    test_play_request_allowed_after_register();
    test_route_on_unknown_connection_is_ignored();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL MESSAGE ROUTER TESTS EXECUTED      " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
