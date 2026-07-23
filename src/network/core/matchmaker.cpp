#include "../../../include/network/core/matchmaker.h"
#include "../../../include/io/BoardParser.h"

Matchmaker::Matchmaker(ConnectionsRegistry& registry, GamesRegistry& games, GameServer& server, std::string boardCsvPath)
    : registry_(registry), games_(games), server_(server), boardCsvPath_(std::move(boardCsvPath)) {}

std::unique_ptr<Board> Matchmaker::freshBoard() const {
    Board board = BoardParser::parseFromCsv(boardCsvPath_);
    return std::make_unique<Board>(std::move(board));
}

void Matchmaker::handlePlayRequest(ConnectionId connectionId) {
    if (!pendingRoom_) {
        pendingRoom_ = std::make_unique<GameRoom>(games_.allocateId(), freshBoard());
    }

    PlayerRole role = pendingRoom_->join(connectionId);
    registry_.setGame(connectionId, pendingRoom_->id(), role);
    registry_.setStatus(connectionId, ConnectionStatus::InGame);

    if (pendingRoom_->hasStarted()) {
        games_.activate(std::move(pendingRoom_), server_);
        pendingRoom_.reset();
    }
}
