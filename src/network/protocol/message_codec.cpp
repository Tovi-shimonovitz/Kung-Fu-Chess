#include "../../../include/network/protocol/message_codec.h"
#include <stdexcept>

namespace {
const char* REGISTER_TYPE = "register";
const char* PLAY_REQUEST_TYPE = "play_request";
const char* MOVE_REQUEST_TYPE = "move_request";
}

std::string MessageCodec::typeToString(MessageType type) {
    switch (type) {
        case MessageType::Register: return REGISTER_TYPE;
        case MessageType::PlayRequest: return PLAY_REQUEST_TYPE;
        case MessageType::MoveRequest: return MOVE_REQUEST_TYPE;
        default: throw std::runtime_error("ERROR UNKNOWN_MESSAGE_TYPE");
    }
}

MessageType MessageCodec::typeFromString(const std::string& text) {
    if (text == REGISTER_TYPE) return MessageType::Register;
    if (text == PLAY_REQUEST_TYPE) return MessageType::PlayRequest;
    if (text == MOVE_REQUEST_TYPE) return MessageType::MoveRequest;
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
    RegisterMessage message;
    message.username = raw.payload.at("username").get<std::string>();
    return message;
}

RawMessage MessageCodec::toRaw(const RegisterMessage& message) {
    RawMessage raw;
    raw.type = MessageType::Register;
    raw.payload["username"] = message.username;
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
    MoveRequestMessage message;
    message.from = Position(raw.payload.at("from").at("row").get<int>(),
                             raw.payload.at("from").at("col").get<int>());
    message.to = Position(raw.payload.at("to").at("row").get<int>(),
                           raw.payload.at("to").at("col").get<int>());
    return message;
}

RawMessage MessageCodec::toRaw(const MoveRequestMessage& message) {
    RawMessage raw;
    raw.type = MessageType::MoveRequest;
    raw.payload["from"] = { {"row", message.from.row}, {"col", message.from.col} };
    raw.payload["to"] = { {"row", message.to.row}, {"col", message.to.col} };
    return raw;
}
