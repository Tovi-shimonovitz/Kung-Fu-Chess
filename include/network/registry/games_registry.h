#pragma once
#include <memory>
#include <unordered_map>
#include "../connection_info.h"
#include "../room/room_worker.h"

class GameServer;

class GamesRegistry {
public:
    RoomWorker& activate(std::unique_ptr<GameRoom> room, GameServer& server);
    RoomWorker* find(GameId id);

private:
    std::unordered_map<GameId, std::unique_ptr<RoomWorker>> workers_;
};
