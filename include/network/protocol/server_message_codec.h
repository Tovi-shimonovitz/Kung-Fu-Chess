#pragma once
#include <string>
#include "server_message.h"
#include "../../engine/Result_structs.h"

class ServerMessageCodec {
public:
    static ServerRawMessage parseRaw(const std::string& text);
    static std::string serializeRaw(const ServerRawMessage& message);

    static GameSnapshot parseGameSnapshot(const ServerRawMessage& raw);
    static ServerRawMessage toRaw(const GameSnapshot& snapshot);

private:
    static std::string typeToString(ServerMessageType type);
    static ServerMessageType typeFromString(const std::string& text);
};
