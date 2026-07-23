#pragma once
#include <asio.hpp>
#include <atomic>
#include <memory>
#include "../connection_info.h"
#include "game_room.h"

class GameServer;

class RoomWorker : public std::enable_shared_from_this<RoomWorker> {
public:
    static std::shared_ptr<RoomWorker> create(asio::io_context& sharedIoContext,
                                               std::unique_ptr<GameRoom> room,
                                               GameServer& server);
    ~RoomWorker();

    RoomWorker(const RoomWorker&) = delete;
    RoomWorker& operator=(const RoomWorker&) = delete;

    void postMoveRequest(ConnectionId connectionId, Position src, Position dst);
    void postAddSpectator(ConnectionId connectionId);

private:
    RoomWorker(asio::io_context& sharedIoContext, std::unique_ptr<GameRoom> room, GameServer& server);

    void scheduleTick();
    void broadcastSnapshot();

    std::unique_ptr<GameRoom> room_;
    GameServer& server_;

    asio::strand<asio::io_context::executor_type> strand_;
    asio::steady_timer tickTimer_;
    std::atomic<bool> stopped_{false};
};
