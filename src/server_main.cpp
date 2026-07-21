#include <cstdint>
#include <iostream>
#include "../include/io/env_config.h"
#include "../include/network/registry/connections_registry.h"
#include "../include/network/core/game_server.h"
#include "../include/network/registry/games_registry.h"
#include "../include/network/core/matchmaker.h"
#include "../include/network/core/message_router.h"

int main() {
    const std::uint16_t port = 9002;

    EnvConfig env = EnvConfig::load(".env");

    ConnectionsRegistry registry;
    GamesRegistry games;
    GameServer server(registry);
    Matchmaker matchmaker(registry, games, server, env.get("BOARD_CSV_PATH"));
    MessageRouter router(registry, matchmaker, games);
    server.setRouter(router);

    std::cout << "Kung-Fu Chess server starting..." << std::endl;
    server.run(port);

    return 0;
}
