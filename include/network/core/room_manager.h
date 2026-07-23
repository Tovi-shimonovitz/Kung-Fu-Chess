#pragma once
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include "../connection_info.h"
#include "../registry/connections_registry.h"
#include "../registry/games_registry.h"
#include "../room/game_room.h"
#include "../../engine/Result_structs.h"

class GameServer;

struct RoomCreationResult {
    GameId gameId;
    GameSnapshot snapshot;
};

struct RoomJoinResult {
    GameId gameId;
    PlayerRole role;
    GameSnapshot snapshot;
};

class RoomManager {
public:
    RoomManager(ConnectionsRegistry& registry, GamesRegistry& games, GameServer& server, std::string boardCsvPath);

    RoomCreationResult createRoom(ConnectionId connectionId);
    std::variant<RoomJoinResult, std::string> joinRoom(ConnectionId connectionId, GameId gameId);

private:
    std::unique_ptr<Board> freshBoard() const;

    ConnectionsRegistry& registry_;
    GamesRegistry& games_;
    GameServer& server_;
    std::string boardCsvPath_;

    std::unordered_map<GameId, std::unique_ptr<GameRoom>> pendingRooms_;
};
