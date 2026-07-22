#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "../../model/Position.h"

enum class MessageType {
    Register,
    PlayRequest,
    MoveRequest
};

struct RawMessage {
    MessageType type;
    nlohmann::json payload;
};

class ClientMessage {
public:
    virtual ~ClientMessage() = default;
    virtual RawMessage toRaw() const = 0;
};

struct RegisterMessage : public ClientMessage {
    std::string username;

    explicit RegisterMessage(std::string username) : username(std::move(username)) {}
    RawMessage toRaw() const override;
};

struct PlayRequestMessage : public ClientMessage {
    PlayRequestMessage() = default;
    RawMessage toRaw() const override;
};

struct MoveRequestMessage : public ClientMessage {
    Position from;
    Position to;

    MoveRequestMessage(Position from, Position to) : from(from), to(to) {}
    RawMessage toRaw() const override;
};
