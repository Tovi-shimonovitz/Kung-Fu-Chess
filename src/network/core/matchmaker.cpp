#include "../../../include/network/core/matchmaker.h"
#include "../../../include/io/BoardParser.h"
#include "../../../include/network/core/game_server.h"
#include <algorithm>
#include <cstdlib>

Matchmaker::Matchmaker(ConnectionsRegistry& registry, GamesRegistry& games, GameServer& server, std::string boardCsvPath)
    : registry_(registry), games_(games), server_(server), boardCsvPath_(std::move(boardCsvPath)) {}

std::unique_ptr<Board> Matchmaker::freshBoard() const {
    Board board = BoardParser::parseFromCsv(boardCsvPath_);
    return std::make_unique<Board>(std::move(board));
}

void Matchmaker::handlePlayRequest(ConnectionId connectionId) {
    auto info = registry_.get(connectionId);
    int rating = info ? info->rating : 1200;

    auto match = std::find_if(waitingQueue_.begin(), waitingQueue_.end(),
        [&](const std::unique_ptr<WaitingPlayer>& waiting) {
            return std::abs(waiting->rating - rating) <= kRatingRange;
        });

    if (match != waitingQueue_.end()) {
        ConnectionId opponentId = (*match)->connectionId;
        (*match)->timeoutTimer.cancel();
        waitingQueue_.erase(match);
        startGame(opponentId, connectionId);
        return;
    }

    auto waiting = std::make_unique<WaitingPlayer>(connectionId, rating, server_.ioContext());
    waiting->timeoutTimer.expires_after(std::chrono::seconds(kWaitSeconds));
    waiting->timeoutTimer.async_wait([this, connectionId](const asio::error_code& ec) {
        onTimeout(connectionId, ec);
    });
    waitingQueue_.push_back(std::move(waiting));
}

void Matchmaker::startGame(ConnectionId firstConnectionId, ConnectionId secondConnectionId) {
    auto room = std::make_unique<GameRoom>(games_.allocateId(), freshBoard());
    GameId gameId = room->id();

    PlayerRole firstRole = room->join(firstConnectionId);
    registry_.setGame(firstConnectionId, gameId, firstRole);
    registry_.setStatus(firstConnectionId, ConnectionStatus::InGame);

    PlayerRole secondRole = room->join(secondConnectionId);
    registry_.setGame(secondConnectionId, gameId, secondRole);
    registry_.setStatus(secondConnectionId, ConnectionStatus::InGame);

    games_.activate(std::move(room), server_);
}

void Matchmaker::onTimeout(ConnectionId connectionId, const asio::error_code& ec) {
    if (ec) return; // timer was cancelled - a match was already found

    auto waiting = std::find_if(waitingQueue_.begin(), waitingQueue_.end(),
        [&](const std::unique_ptr<WaitingPlayer>& w) { return w->connectionId == connectionId; });
    if (waiting == waitingQueue_.end()) return;

    waitingQueue_.erase(waiting);
    server_.send(connectionId, "ERROR: no opponent found");
}
