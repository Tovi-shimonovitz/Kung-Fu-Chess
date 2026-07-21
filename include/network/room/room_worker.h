#pragma once
#include <asio.hpp>
#include <memory>
#include <thread>
#include "../connection_info.h"
#include "game_room.h"

class GameServer;

class RoomWorker {
public:
    RoomWorker(std::unique_ptr<GameRoom> room, GameServer& server);
    ~RoomWorker();

    void postMoveRequest(ConnectionId connectionId, Position src, Position dst);

private:
    void scheduleTick();
    void broadcastSnapshot();

    std::unique_ptr<GameRoom> room_;
    GameServer& server_;

    asio::io_context ioContext_;
    asio::steady_timer tickTimer_;
    std::thread thread_;
};
