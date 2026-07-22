#pragma once
#include <optional>
#include <string>
#include "../connection_info.h"
#include "../registry/connections_registry.h"
#include "../protocol/messages.h"

class Matchmaker;
class GamesRegistry;
class AuthHandler;

class MessageRouter {
public:
    MessageRouter(ConnectionsRegistry& registry, Matchmaker& matchmaker, GamesRegistry& games, AuthHandler& authHandler);

    std::optional<std::string> route(ConnectionId connectionId, const RawMessage& raw);

private:
    ConnectionsRegistry& registry_;
    Matchmaker& matchmaker_;
    GamesRegistry& games_;
    AuthHandler& authHandler_;

    bool isAllowed(ConnectionStatus status, MessageType type) const;

    std::optional<std::string> handleRegister(ConnectionId connectionId, const RawMessage& raw);
    void handlePlayRequest(ConnectionId connectionId, const RawMessage& raw);
    void handleMoveRequest(ConnectionId connectionId, const RawMessage& raw);
};
