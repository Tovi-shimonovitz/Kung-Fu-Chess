#pragma once
#include <memory>
#include <thread>
#include <unordered_map>
#include "../connection_info.h"
#include "../room/game_worker_pool.h"
#include "../room/room_worker.h"

class GameServer;

class GamesRegistry {
public:
    explicit GamesRegistry(std::size_t threadCount = std::thread::hardware_concurrency());

    std::shared_ptr<RoomWorker> activate(std::unique_ptr<GameRoom> room, GameServer& server);
    std::shared_ptr<RoomWorker> find(GameId id);

private:
    GameWorkerPool pool_;
    std::unordered_map<GameId, std::shared_ptr<RoomWorker>> workers_;
};
