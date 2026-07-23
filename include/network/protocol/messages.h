#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "../../model/Position.h"
#include "../connection_info.h"

enum class MessageType {
    Register,
    PlayRequest,
    MoveRequest,
    CreateRoom,
    JoinRoom
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
    std::string password;

    RegisterMessage(std::string username, std::string password)
        : username(std::move(username)), password(std::move(password)) {}
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

struct CreateRoomMessage : public ClientMessage {
    CreateRoomMessage() = default;
    RawMessage toRaw() const override;
};

struct JoinRoomMessage : public ClientMessage {
    GameId gameId;

    explicit JoinRoomMessage(GameId gameId) : gameId(gameId) {}
    RawMessage toRaw() const override;
};
