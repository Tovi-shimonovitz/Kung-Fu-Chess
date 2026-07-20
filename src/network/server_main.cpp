#include <cstdint>
#include <iostream>
#include "../../include/network/connections_registry.h"
#include "../../include/network/game_server.h"
#include "../../include/network/message_router.h"

int main() {
    const std::uint16_t port = 9002;

    ConnectionsRegistry registry;
    MessageRouter router(registry);
    GameServer server(registry, router);

    std::cout << "Kung-Fu Chess server starting..." << std::endl;
    server.run(port);

    return 0;
}
