#pragma once
#include <nlohmann/json.hpp>
#include "../connection_info.h"
#include "../../engine/Result_structs.h"

enum class ServerMessageType {
    GameSnapshot,
    RoomCreated
};

struct ServerRawMessage {
    ServerMessageType type;
    nlohmann::json payload;
};

struct RoomCreatedMessage {
    GameId gameId;
    GameSnapshot snapshot;
};
