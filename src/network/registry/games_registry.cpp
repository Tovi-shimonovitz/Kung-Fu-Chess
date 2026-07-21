#include "../../../include/network/registry/games_registry.h"
#include "../../../include/network/core/game_server.h"

RoomWorker& GamesRegistry::activate(std::unique_ptr<GameRoom> room, GameServer& server) {
    GameId id = room->id();
    auto worker = std::make_unique<RoomWorker>(std::move(room), server);
    RoomWorker& ref = *worker;
    workers_[id] = std::move(worker);
    return ref;
}

RoomWorker* GamesRegistry::find(GameId id) {
    auto it = workers_.find(id);
    return it != workers_.end() ? it->second.get() : nullptr;
}
