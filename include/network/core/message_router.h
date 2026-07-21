#pragma once
#include <optional>
#include <string>
#include "../connection_info.h"
#include "../registry/connections_registry.h"
#include "../protocol/messages.h"
#include "../protocol/raw_message.h"

class Matchmaker;
class GamesRegistry;

class MessageRouter {
public:
    MessageRouter(ConnectionsRegistry& registry, Matchmaker& matchmaker, GamesRegistry& games);

    std::optional<std::string> route(ConnectionId connectionId, const RawMessage& raw);

private:
    ConnectionsRegistry& registry_;
    Matchmaker& matchmaker_;
    GamesRegistry& games_;

    bool isAllowed(ConnectionStatus status, MessageType type) const;

    void handleRegister(ConnectionId connectionId, const RawMessage& raw);
    void handlePlayRequest(ConnectionId connectionId, const RawMessage& raw);
    void handleMoveRequest(ConnectionId connectionId, const RawMessage& raw);
};
