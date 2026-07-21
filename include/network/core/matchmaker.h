#pragma once
#include <memory>
#include <string>
#include "../connection_info.h"
#include "../registry/connections_registry.h"
#include "../room/game_room.h"
#include "../registry/games_registry.h"

class GameServer;

class Matchmaker {
public:
    Matchmaker(ConnectionsRegistry& registry, GamesRegistry& games, GameServer& server, std::string boardCsvPath);

    void handlePlayRequest(ConnectionId connectionId);

private:
    std::unique_ptr<Board> freshBoard() const;

    ConnectionsRegistry& registry_;
    GamesRegistry& games_;
    GameServer& server_;
    std::string boardCsvPath_;

    std::unique_ptr<GameRoom> pendingRoom_;
    GameId nextGameId_ = 1;
};
