#include "../../../include/network/registry/connections_registry.h"

void ConnectionsRegistry::add(ConnectionId id) {
    connections_[id] = ConnectionInfo{id};
}

void ConnectionsRegistry::remove(ConnectionId id) {
    connections_.erase(id);
}

std::optional<ConnectionInfo> ConnectionsRegistry::get(ConnectionId id) const {
    auto it = connections_.find(id);
    if (it == connections_.end()) return std::nullopt;
    return it->second;
}

void ConnectionsRegistry::setStatus(ConnectionId id, ConnectionStatus status) {
    auto it = connections_.find(id);
    if (it != connections_.end()) it->second.status = status;
}

void ConnectionsRegistry::setUsername(ConnectionId id, const std::string& username) {
    auto it = connections_.find(id);
    if (it != connections_.end()) it->second.username = username;
}

void ConnectionsRegistry::setGame(ConnectionId id, GameId gameId, PlayerRole role) {
    auto it = connections_.find(id);
    if (it != connections_.end()) {
        it->second.gameId = gameId;
        it->second.role = role;
    }
}
