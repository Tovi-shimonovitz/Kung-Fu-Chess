#include "../../../include/network/registry/games_registry.h"
#include "../../../include/network/core/game_server.h"

GamesRegistry::GamesRegistry(std::size_t threadCount) : pool_(threadCount) {}

std::shared_ptr<RoomWorker> GamesRegistry::activate(std::unique_ptr<GameRoom> room, GameServer& server) {
    GameId id = room->id();
    auto worker = RoomWorker::create(pool_.ioContext(), std::move(room), server);
    workers_[id] = worker;
    return worker;
}

std::shared_ptr<RoomWorker> GamesRegistry::find(GameId id) {
    auto it = workers_.find(id);
    return it != workers_.end() ? it->second : nullptr;
}
