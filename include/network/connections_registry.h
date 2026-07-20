#pragma once
#include <optional>
#include <unordered_map>
#include "connection_info.h"

class ConnectionsRegistry {
public:
    void add(ConnectionId id);
    void remove(ConnectionId id);

    std::optional<ConnectionInfo> get(ConnectionId id) const;

    void setStatus(ConnectionId id, ConnectionStatus status);
    void setUsername(ConnectionId id, const std::string& username);
    void setGame(ConnectionId id, GameId gameId, PlayerRole role);

private:
    std::unordered_map<ConnectionId, ConnectionInfo> connections_;
};
