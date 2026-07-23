#pragma once
#include <asio.hpp>
#include <memory>
#include <string>
#include <vector>
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
    struct WaitingPlayer {
        ConnectionId connectionId;
        int rating;
        asio::steady_timer timeoutTimer;

        WaitingPlayer(ConnectionId connectionId, int rating, asio::io_context& ioContext)
            : connectionId(connectionId), rating(rating), timeoutTimer(ioContext) {}
    };

    static constexpr int kRatingRange = 100;
    static constexpr int kWaitSeconds = 60;

    std::unique_ptr<Board> freshBoard() const;
    void startGame(ConnectionId firstConnectionId, ConnectionId secondConnectionId);
    void onTimeout(ConnectionId connectionId, const asio::error_code& ec);

    ConnectionsRegistry& registry_;
    GamesRegistry& games_;
    GameServer& server_;
    std::string boardCsvPath_;

    std::vector<std::unique_ptr<WaitingPlayer>> waitingQueue_;
};
