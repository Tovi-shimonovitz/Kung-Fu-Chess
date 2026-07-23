#pragma once
#include <cstdint>
#include <optional>
#include <string>

using ConnectionId = std::uint64_t;
using GameId = std::uint64_t;

enum class ConnectionStatus {
    Connected,
    Registered,
    InGame
};

enum class PlayerRole {
    None,
    White,
    Black,
    Spectator
};

struct ConnectionInfo {
    ConnectionId id;
    ConnectionStatus status = ConnectionStatus::Connected;
    std::string username;
    std::optional<GameId> gameId;
    PlayerRole role = PlayerRole::None;
    int rating = 1200;
};
