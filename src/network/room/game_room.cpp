#include "../../../include/network/room/game_room.h"

GameRoom::GameRoom(GameId id, std::unique_ptr<Board> board) : id_(id) {
    engine_.setBoard(std::move(board));
}

GameId GameRoom::id() const {
    return id_;
}

PlayerRole GameRoom::join(ConnectionId connectionId) {
    if (!whiteId_) {
        whiteId_ = connectionId;
        return PlayerRole::White;
    }
    if (!blackId_) {
        blackId_ = connectionId;
        return PlayerRole::Black;
    }
    spectatorIds_.push_back(connectionId);
    return PlayerRole::Spectator;
}

void GameRoom::addSpectator(ConnectionId connectionId) {
    spectatorIds_.push_back(connectionId);
}

bool GameRoom::hasStarted() const {
    return whiteId_.has_value() && blackId_.has_value();
}

std::optional<ConnectionId> GameRoom::whiteConnectionId() const { return whiteId_; }
std::optional<ConnectionId> GameRoom::blackConnectionId() const { return blackId_; }
const std::vector<ConnectionId>& GameRoom::spectatorIds() const { return spectatorIds_; }

PieceColor GameRoom::roleColor(ConnectionId connectionId) const {
    if (whiteId_ && *whiteId_ == connectionId) return PieceColor::WHITE;
    if (blackId_ && *blackId_ == connectionId) return PieceColor::BLACK;
    return PieceColor::None;
}

MoveResult GameRoom::requestMove(ConnectionId connectionId, Position src, Position dst) {
    if (!hasStarted()) {
        return {false, "game_not_started"};
    }
    PieceColor role = roleColor(connectionId);
    if (role == PieceColor::None) {
        return {false, "not_a_player_in_this_game"};
    }
    if (engine_.colorAt(src) != role) {
        return {false, "not_your_piece"};
    }
    return engine_.requestMove(src, dst);
}

void GameRoom::tick(int elapsedMs) {
    engine_.wait(elapsedMs);
}

GameSnapshot GameRoom::snapshot() const {
    return engine_.getSnapshot();
}
