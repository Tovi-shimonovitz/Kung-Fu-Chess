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
#include "../../include/network/core/room_manager.h"
#include "../../include/network/protocol/server_message_codec.h"
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
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

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
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

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
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

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
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

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
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);
    router.route(1, MessageCodec::toRaw(RegisterMessage{"bob", "pw"}));

    auto error = router.route(1, MessageCodec::toRaw(PlayRequestMessage{}));

    assert(!error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_play_request_allowed_after_register");
}

void test_route_on_unknown_connection_is_ignored() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

    auto error = router.route(999, MessageCodec::toRaw(RegisterMessage{"ghost", "pw"}));

    assert(!error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_route_on_unknown_connection_is_ignored");
}

void test_create_room_rejected_before_register() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

    auto error = router.route(1, MessageCodec::toRaw(CreateRoomMessage{}));

    assert(error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_create_room_rejected_before_register");
}

void test_create_then_join_room_makes_second_player_black() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    registry.add(2);
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

    router.route(1, MessageCodec::toRaw(RegisterMessage{"alice", "pw"}));
    router.route(2, MessageCodec::toRaw(RegisterMessage{"bob", "pw"}));

    auto createReply = router.route(1, MessageCodec::toRaw(CreateRoomMessage{}));
    assert(createReply.has_value());
    assert(registry.get(1)->role == PlayerRole::White);

    ServerRawMessage createRaw = ServerMessageCodec::parseRaw(*createReply);
    RoomCreatedMessage created = ServerMessageCodec::parseRoomCreated(createRaw);
    assert(!created.snapshot.pieces.empty());

    auto joinError = router.route(2, MessageCodec::toRaw(JoinRoomMessage{created.gameId}));
    assert(!joinError.has_value());
    assert(registry.get(2)->role == PlayerRole::Black);
    assert(registry.get(2)->status == ConnectionStatus::InGame);

    std::remove(TEST_DB_PATH);
    log_test("test_create_then_join_room_makes_second_player_black");
}

void test_join_room_rejects_unknown_id() {
    std::remove(TEST_DB_PATH);
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games(1);
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, boardCsvPath());
    UserRepository userRepository(TEST_DB_PATH);
    AuthHandler authHandler(userRepository);
    RoomManager roomManager(registry, games, server, boardCsvPath());
    MessageRouter router(registry, matchmaker, games, authHandler, roomManager);

    router.route(1, MessageCodec::toRaw(RegisterMessage{"alice", "pw"}));
    auto error = router.route(1, MessageCodec::toRaw(JoinRoomMessage{9999}));

    assert(error.has_value());
    std::remove(TEST_DB_PATH);
    log_test("test_join_room_rejects_unknown_id");
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
    test_create_room_rejected_before_register();
    test_create_then_join_room_makes_second_player_black();
    test_join_room_rejects_unknown_id();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL MESSAGE ROUTER TESTS EXECUTED      " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
