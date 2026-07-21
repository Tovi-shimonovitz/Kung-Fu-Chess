#pragma once
#include <nlohmann/json.hpp>

enum class ServerMessageType {
    GameSnapshot
};

struct ServerRawMessage {
    ServerMessageType type;
    nlohmann::json payload;
};
