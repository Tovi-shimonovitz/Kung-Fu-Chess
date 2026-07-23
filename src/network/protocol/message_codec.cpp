#include "../../../include/network/protocol/message_codec.h"
#include <stdexcept>

namespace {
const char* REGISTER_TYPE = "register";
const char* PLAY_REQUEST_TYPE = "play_request";
const char* MOVE_REQUEST_TYPE = "move_request";
const char* CREATE_ROOM_TYPE = "create_room";
const char* JOIN_ROOM_TYPE = "join_room";
}

std::string MessageCodec::typeToString(MessageType type) {
    switch (type) {
        case MessageType::Register: return REGISTER_TYPE;
        case MessageType::PlayRequest: return PLAY_REQUEST_TYPE;
        case MessageType::MoveRequest: return MOVE_REQUEST_TYPE;
        case MessageType::CreateRoom: return CREATE_ROOM_TYPE;
        case MessageType::JoinRoom: return JOIN_ROOM_TYPE;
        default: throw std::runtime_error("ERROR UNKNOWN_MESSAGE_TYPE");
    }
}

MessageType MessageCodec::typeFromString(const std::string& text) {
    if (text == REGISTER_TYPE) return MessageType::Register;
    if (text == PLAY_REQUEST_TYPE) return MessageType::PlayRequest;
    if (text == MOVE_REQUEST_TYPE) return MessageType::MoveRequest;
    if (text == CREATE_ROOM_TYPE) return MessageType::CreateRoom;
    if (text == JOIN_ROOM_TYPE) return MessageType::JoinRoom;
    throw std::runtime_error("ERROR UNKNOWN_MESSAGE_TYPE: " + text);
}

RawMessage MessageCodec::parseRaw(const std::string& text) {
    nlohmann::json data = nlohmann::json::parse(text);
    RawMessage raw;
    raw.type = typeFromString(data.at("type").get<std::string>());
    raw.payload = data.at("payload");
    return raw;
}

std::string MessageCodec::serializeRaw(const RawMessage& message) {
    nlohmann::json data;
    data["type"] = typeToString(message.type);
    data["payload"] = message.payload;
    return data.dump();
}

RegisterMessage MessageCodec::parseRegister(const RawMessage& raw) {
    return RegisterMessage(
        raw.payload.at("username").get<std::string>(),
        raw.payload.at("password").get<std::string>());
}

RawMessage MessageCodec::toRaw(const RegisterMessage& message) {
    RawMessage raw;
    raw.type = MessageType::Register;
    raw.payload["username"] = message.username;
    raw.payload["password"] = message.password;
    return raw;
}

PlayRequestMessage MessageCodec::parsePlayRequest(const RawMessage& raw) {
    (void)raw;
    return PlayRequestMessage{};
}

RawMessage MessageCodec::toRaw(const PlayRequestMessage& message) {
    (void)message;
    RawMessage raw;
    raw.type = MessageType::PlayRequest;
    raw.payload = nlohmann::json::object();
    return raw;
}

MoveRequestMessage MessageCodec::parseMoveRequest(const RawMessage& raw) {
    Position from(raw.payload.at("from").at("row").get<int>(),
                  raw.payload.at("from").at("col").get<int>());
    Position to(raw.payload.at("to").at("row").get<int>(),
                raw.payload.at("to").at("col").get<int>());
    return MoveRequestMessage(from, to);
}

RawMessage MessageCodec::toRaw(const MoveRequestMessage& message) {
    RawMessage raw;
    raw.type = MessageType::MoveRequest;
    raw.payload["from"] = { {"row", message.from.row}, {"col", message.from.col} };
    raw.payload["to"] = { {"row", message.to.row}, {"col", message.to.col} };
    return raw;
}

CreateRoomMessage MessageCodec::parseCreateRoom(const RawMessage& raw) {
    (void)raw;
    return CreateRoomMessage{};
}

RawMessage MessageCodec::toRaw(const CreateRoomMessage& message) {
    (void)message;
    RawMessage raw;
    raw.type = MessageType::CreateRoom;
    raw.payload = nlohmann::json::object();
    return raw;
}

JoinRoomMessage MessageCodec::parseJoinRoom(const RawMessage& raw) {
    return JoinRoomMessage(raw.payload.at("gameId").get<GameId>());
}

RawMessage MessageCodec::toRaw(const JoinRoomMessage& message) {
    RawMessage raw;
    raw.type = MessageType::JoinRoom;
    raw.payload["gameId"] = message.gameId;
    return raw;
}

RawMessage RegisterMessage::toRaw() const { return MessageCodec::toRaw(*this); }
RawMessage PlayRequestMessage::toRaw() const { return MessageCodec::toRaw(*this); }
RawMessage MoveRequestMessage::toRaw() const { return MessageCodec::toRaw(*this); }
RawMessage CreateRoomMessage::toRaw() const { return MessageCodec::toRaw(*this); }
RawMessage JoinRoomMessage::toRaw() const { return MessageCodec::toRaw(*this); }
