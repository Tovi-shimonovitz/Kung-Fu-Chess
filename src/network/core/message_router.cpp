#include "../../../include/network/core/message_router.h"
#include "../../../include/network/protocol/message_codec.h"
#include "../../../include/network/core/matchmaker.h"
#include "../../../include/network/registry/games_registry.h"
#include "../../../include/network/room/room_worker.h"
#include <iostream>

MessageRouter::MessageRouter(ConnectionsRegistry& registry, Matchmaker& matchmaker, GamesRegistry& games)
    : registry_(registry), matchmaker_(matchmaker), games_(games) {}

bool MessageRouter::isAllowed(ConnectionStatus status, MessageType type) const {
    switch (status) {
        case ConnectionStatus::Connected: return type == MessageType::Register;
        case ConnectionStatus::Registered: return type == MessageType::PlayRequest;
        case ConnectionStatus::InGame: return type == MessageType::MoveRequest;
    }
    return false;
}

std::optional<std::string> MessageRouter::route(ConnectionId connectionId, const RawMessage& raw) {
    auto info = registry_.get(connectionId);
    if (!info) return std::nullopt;

    if (!isAllowed(info->status, raw.type)) {
        return "ERROR: message not allowed in current state";
    }

    switch (raw.type) {
        case MessageType::Register: handleRegister(connectionId, raw); break;
        case MessageType::PlayRequest: handlePlayRequest(connectionId, raw); break;
        case MessageType::MoveRequest: handleMoveRequest(connectionId, raw); break;
    }
    return std::nullopt;
}

void MessageRouter::handleRegister(ConnectionId connectionId, const RawMessage& raw) {
    RegisterMessage message = MessageCodec::parseRegister(raw);
    registry_.setUsername(connectionId, message.username);
    registry_.setStatus(connectionId, ConnectionStatus::Registered);
    std::cout << "[Router] " << message.username << " registered (connection "
              << connectionId << ")" << std::endl;
}

void MessageRouter::handlePlayRequest(ConnectionId connectionId, const RawMessage& raw) {
    MessageCodec::parsePlayRequest(raw);
    matchmaker_.handlePlayRequest(connectionId);
}

void MessageRouter::handleMoveRequest(ConnectionId connectionId, const RawMessage& raw) {
    MoveRequestMessage message = MessageCodec::parseMoveRequest(raw);

    auto info = registry_.get(connectionId);
    if (!info || !info->gameId) return;

    RoomWorker* room = games_.find(*info->gameId);
    if (!room) return;

    room->postMoveRequest(connectionId, message.from, message.to);
}
