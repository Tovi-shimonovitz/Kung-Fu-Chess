#pragma once
#include <optional>
#include <string>
#include "connection_info.h"
#include "connections_registry.h"
#include "messages.h"
#include "raw_message.h"

class MessageRouter {
public:
    explicit MessageRouter(ConnectionsRegistry& registry);

    std::optional<std::string> route(ConnectionId connectionId, const RawMessage& raw);

private:
    ConnectionsRegistry& registry_;

    bool isAllowed(ConnectionStatus status, MessageType type) const;

    void handleRegister(ConnectionId connectionId, const RawMessage& raw);
    void handlePlayRequest(ConnectionId connectionId, const RawMessage& raw);
    void handleMoveRequest(ConnectionId connectionId, const RawMessage& raw);
};
