#include "../../../include/network/core/room_manager.h"
#include "../../../include/io/BoardParser.h"

RoomManager::RoomManager(ConnectionsRegistry& registry, GamesRegistry& games, GameServer& server, std::string boardCsvPath)
    : registry_(registry), games_(games), server_(server), boardCsvPath_(std::move(boardCsvPath)) {}

std::unique_ptr<Board> RoomManager::freshBoard() const {
    Board board = BoardParser::parseFromCsv(boardCsvPath_);
    return std::make_unique<Board>(std::move(board));
}

RoomCreationResult RoomManager::createRoom(ConnectionId connectionId) {
    GameId gameId = games_.allocateId();
    auto room = std::make_unique<GameRoom>(gameId, freshBoard());
    PlayerRole role = room->join(connectionId);
    GameSnapshot snapshot = room->snapshot();

    registry_.setGame(connectionId, gameId, role);
    registry_.setStatus(connectionId, ConnectionStatus::InGame);

    pendingRooms_[gameId] = std::move(room);
    return RoomCreationResult{gameId, snapshot};
}

std::variant<RoomJoinResult, std::string> RoomManager::joinRoom(ConnectionId connectionId, GameId gameId) {
    auto pendingIt = pendingRooms_.find(gameId);
    if (pendingIt != pendingRooms_.end()) {
        PlayerRole role = pendingIt->second->join(connectionId);
        registry_.setGame(connectionId, gameId, role);
        registry_.setStatus(connectionId, ConnectionStatus::InGame);
        GameSnapshot snapshot = pendingIt->second->snapshot();

        if (pendingIt->second->hasStarted()) {
            games_.activate(std::move(pendingIt->second), server_);
            pendingRooms_.erase(pendingIt);
        }
        return RoomJoinResult{gameId, role, snapshot};
    }

    if (auto worker = games_.find(gameId)) {
        registry_.setGame(connectionId, gameId, PlayerRole::Spectator);
        registry_.setStatus(connectionId, ConnectionStatus::InGame);
        worker->postAddSpectator(connectionId);
        return RoomJoinResult{gameId, PlayerRole::Spectator, GameSnapshot{}};
    }

    return std::string("ERROR: room not found");
}
