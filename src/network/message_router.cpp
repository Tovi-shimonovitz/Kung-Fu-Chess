#include "../../include/network/message_router.h"
#include "../../include/network/message_codec.h"
#include <iostream>

MessageRouter::MessageRouter(ConnectionsRegistry& registry) : registry_(registry) {}

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
    std::cout << "[Router] connection " << connectionId
              << " requested to play (matchmaking not implemented yet)" << std::endl;
}

void MessageRouter::handleMoveRequest(ConnectionId connectionId, const RawMessage& raw) {
    MoveRequestMessage message = MessageCodec::parseMoveRequest(raw);
    std::cout << "[Router] connection " << connectionId << " move from ("
              << message.from.row << "," << message.from.col << ") to ("
              << message.to.row << "," << message.to.col
              << ") (game logic not wired yet)" << std::endl;
}
