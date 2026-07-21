#include "../../../include/network/room/room_worker.h"
#include "../../../include/network/core/game_server.h"
#include "../../../include/network/protocol/server_message_codec.h"
#include "../../../include/realtime/real_time_arbiter.h"

RoomWorker::RoomWorker(std::unique_ptr<GameRoom> room, GameServer& server)
    : room_(std::move(room)), server_(server), tickTimer_(ioContext_) {
    scheduleTick();
    thread_ = std::thread([this] { ioContext_.run(); });
}

RoomWorker::~RoomWorker() {
    ioContext_.stop();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void RoomWorker::scheduleTick() {
    tickTimer_.expires_after(std::chrono::milliseconds(RealTimeArbiter::TICK_MS));
    tickTimer_.async_wait([this](const asio::error_code& ec) {
        if (ec) return;
        room_->tick(RealTimeArbiter::TICK_MS);
        broadcastSnapshot();
        scheduleTick();
    });
}

void RoomWorker::broadcastSnapshot() {
    ServerRawMessage raw = ServerMessageCodec::toRaw(room_->snapshot());
    std::string text = ServerMessageCodec::serializeRaw(raw);

    if (auto white = room_->whiteConnectionId()) server_.send(*white, text);
    if (auto black = room_->blackConnectionId()) server_.send(*black, text);
    for (ConnectionId spectatorId : room_->spectatorIds()) {
        server_.send(spectatorId, text);
    }
}

void RoomWorker::postMoveRequest(ConnectionId connectionId, Position src, Position dst) {
    asio::post(ioContext_, [this, connectionId, src, dst] {
        MoveResult result = room_->requestMove(connectionId, src, dst);
        if (!result.is_accepted) {
            server_.send(connectionId, "ERROR: " + result.reason);
        }
    });
}
