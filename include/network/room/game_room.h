#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "../../engine/GameEngine.h"
#include "../../model/Piece_enums.h"
#include "../../model/Position.h"
#include "../connection_info.h"

class GameRoom {
public:
    GameRoom(GameId id, std::unique_ptr<Board> board);

    GameId id() const;
    PlayerRole join(ConnectionId connectionId);
    bool hasStarted() const;

    std::optional<ConnectionId> whiteConnectionId() const;
    std::optional<ConnectionId> blackConnectionId() const;
    const std::vector<ConnectionId>& spectatorIds() const;

    MoveResult requestMove(ConnectionId connectionId, Position src, Position dst);

    void tick(int elapsedMs);
    GameSnapshot snapshot() const;

private:
    GameId id_;
    std::optional<ConnectionId> whiteId_;
    std::optional<ConnectionId> blackId_;
    std::vector<ConnectionId> spectatorIds_;
    GameEngine engine_;

    PieceColor roleColor(ConnectionId connectionId) const;
};
