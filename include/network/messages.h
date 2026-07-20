#pragma once
#include <string>
#include "../model/Position.h"

enum class MessageType {
    Register,
    PlayRequest,
    MoveRequest
};

struct RegisterMessage {
    std::string username;
};

struct PlayRequestMessage {
};

struct MoveRequestMessage {
    Position from;
    Position to;
};
