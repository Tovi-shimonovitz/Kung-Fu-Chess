#include "../../../include/network/room/room_worker.h"
#include "../../../include/network/core/game_server.h"
#include "../../../include/network/protocol/server_message_codec.h"
#include "../../../include/realtime/real_time_arbiter.h"

RoomWorker::RoomWorker(asio::io_context& sharedIoContext, std::unique_ptr<GameRoom> room, GameServer& server)
    : room_(std::move(room)), server_(server),
      strand_(asio::make_strand(sharedIoContext)), tickTimer_(sharedIoContext) {}

std::shared_ptr<RoomWorker> RoomWorker::create(asio::io_context& sharedIoContext,
                                                std::unique_ptr<GameRoom> room,
                                                GameServer& server) {
    auto worker = std::shared_ptr<RoomWorker>(new RoomWorker(sharedIoContext, std::move(room), server));
    worker->scheduleTick();
    return worker;
}

RoomWorker::~RoomWorker() {
    stopped_ = true;
    tickTimer_.cancel();
}

void RoomWorker::scheduleTick() {
    std::weak_ptr<RoomWorker> self = weak_from_this();
    tickTimer_.expires_after(std::chrono::milliseconds(RealTimeArbiter::TICK_MS));
    tickTimer_.async_wait(asio::bind_executor(strand_, [self](const asio::error_code& ec) {
        auto shared = self.lock();
        if (!shared || shared->stopped_ || ec) return;
        shared->room_->tick(RealTimeArbiter::TICK_MS);
        shared->broadcastSnapshot();
        shared->scheduleTick();
    }));
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
    auto self = shared_from_this();
    asio::post(strand_, [self, connectionId, src, dst] {
        if (self->stopped_) return;
        MoveResult result = self->room_->requestMove(connectionId, src, dst);
        if (!result.is_accepted) {
            self->server_.send(connectionId, "ERROR: " + result.reason);
        }
    });
}
