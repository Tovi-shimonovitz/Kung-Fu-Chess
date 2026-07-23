#include <cassert>
#include <iostream>
#include <variant>
#include "../../include/network/core/room_manager.h"
#include "../../include/network/core/game_server.h"
#include "../../include/io/env_config.h"

namespace {
std::string boardCsvPath() {
    return EnvConfig::load(".env").get("BOARD_CSV_PATH");
}
}

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_create_room_assigns_white_and_returns_snapshot() {
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games(1);
    GameServer server(registry);
    RoomManager rooms(registry, games, server, boardCsvPath());

    RoomCreationResult created = rooms.createRoom(1);

    auto info = registry.get(1);
    assert(info->role == PlayerRole::White);
    assert(info->status == ConnectionStatus::InGame);
    assert(info->gameId == created.gameId);
    assert(!created.snapshot.pieces.empty());
    log_test("test_create_room_assigns_white_and_returns_snapshot");
}

void test_create_room_generates_distinct_ids() {
    ConnectionsRegistry registry;
    registry.add(1);
    registry.add(2);
    GamesRegistry games(1);
    GameServer server(registry);
    RoomManager rooms(registry, games, server, boardCsvPath());

    RoomCreationResult first = rooms.createRoom(1);
    RoomCreationResult second = rooms.createRoom(2);

    assert(first.gameId != second.gameId);
    log_test("test_create_room_generates_distinct_ids");
}

void test_join_room_rejects_unknown_id() {
    ConnectionsRegistry registry;
    registry.add(1);
    GamesRegistry games(1);
    GameServer server(registry);
    RoomManager rooms(registry, games, server, boardCsvPath());

    auto result = rooms.joinRoom(1, 9999);

    assert(std::holds_alternative<std::string>(result));
    log_test("test_join_room_rejects_unknown_id");
}

void test_second_joiner_becomes_black_and_activates_room() {
    ConnectionsRegistry registry;
    registry.add(1);
    registry.add(2);
    GamesRegistry games(1);
    GameServer server(registry);
    RoomManager rooms(registry, games, server, boardCsvPath());

    RoomCreationResult created = rooms.createRoom(1);
    auto result = rooms.joinRoom(2, created.gameId);

    assert(std::holds_alternative<RoomJoinResult>(result));
    assert(std::get<RoomJoinResult>(result).role == PlayerRole::Black);
    auto info = registry.get(2);
    assert(info->role == PlayerRole::Black);
    assert(info->status == ConnectionStatus::InGame);
    assert(info->gameId == created.gameId);
    log_test("test_second_joiner_becomes_black_and_activates_room");
}

void test_third_joiner_becomes_spectator_after_room_is_active() {
    ConnectionsRegistry registry;
    registry.add(1);
    registry.add(2);
    registry.add(3);
    GamesRegistry games(1);
    GameServer server(registry);
    RoomManager rooms(registry, games, server, boardCsvPath());

    RoomCreationResult created = rooms.createRoom(1);
    rooms.joinRoom(2, created.gameId);
    auto result = rooms.joinRoom(3, created.gameId);

    assert(std::holds_alternative<RoomJoinResult>(result));
    assert(std::get<RoomJoinResult>(result).role == PlayerRole::Spectator);
    auto info = registry.get(3);
    assert(info->role == PlayerRole::Spectator);
    assert(info->status == ConnectionStatus::InGame);
    assert(info->gameId == created.gameId);
    log_test("test_third_joiner_becomes_spectator_after_room_is_active");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING ROOM MANAGER UNIT TESTS       " << std::endl;
    std::cout << "========================================" << std::endl;

    test_create_room_assigns_white_and_returns_snapshot();
    test_create_room_generates_distinct_ids();
    test_join_room_rejects_unknown_id();
    test_second_joiner_becomes_black_and_activates_room();
    test_third_joiner_becomes_spectator_after_room_is_active();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL ROOM MANAGER TESTS EXECUTED        " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
