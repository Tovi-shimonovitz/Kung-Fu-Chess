#include <cassert>
#include <iostream>
#include "../../include/network/registry/connections_registry.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_add_creates_connected_status() {
    ConnectionsRegistry registry;
    registry.add(1);

    auto info = registry.get(1);
    assert(info.has_value());
    assert(info->status == ConnectionStatus::Connected);
    assert(info->username.empty());
    log_test("test_add_creates_connected_status");
}

void test_remove_erases_connection() {
    ConnectionsRegistry registry;
    registry.add(1);
    registry.remove(1);

    assert(!registry.get(1).has_value());
    log_test("test_remove_erases_connection");
}

void test_set_username_and_status() {
    ConnectionsRegistry registry;
    registry.add(1);
    registry.setUsername(1, "alice");
    registry.setStatus(1, ConnectionStatus::Registered);

    auto info = registry.get(1);
    assert(info.has_value());
    assert(info->username == "alice");
    assert(info->status == ConnectionStatus::Registered);
    log_test("test_set_username_and_status");
}

void test_set_game_assigns_role_and_id() {
    ConnectionsRegistry registry;
    registry.add(1);
    registry.setGame(1, 42, PlayerRole::White);

    auto info = registry.get(1);
    assert(info.has_value());
    assert(info->gameId.has_value());
    assert(*info->gameId == 42);
    assert(info->role == PlayerRole::White);
    log_test("test_set_game_assigns_role_and_id");
}

void test_update_on_missing_connection_is_noop() {
    ConnectionsRegistry registry;
    registry.setStatus(999, ConnectionStatus::InGame);
    registry.setUsername(999, "ghost");

    assert(!registry.get(999).has_value());
    log_test("test_update_on_missing_connection_is_noop");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING CONNECTIONS REGISTRY UNIT TESTS" << std::endl;
    std::cout << "========================================" << std::endl;

    test_add_creates_connected_status();
    test_remove_erases_connection();
    test_set_username_and_status();
    test_set_game_assigns_role_and_id();
    test_update_on_missing_connection_is_noop();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL CONNECTIONS REGISTRY TESTS EXECUTED" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
