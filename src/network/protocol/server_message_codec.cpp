#include "../../../include/network/protocol/server_message_codec.h"
#include <stdexcept>

namespace {
const char* GAME_SNAPSHOT_TYPE = "game_snapshot";
const char* ROOM_CREATED_TYPE = "room_created";

std::string colorToString(PieceColor color) {
    switch (color) {
        case PieceColor::WHITE: return "white";
        case PieceColor::BLACK: return "black";
        default: return "none";
    }
}

PieceColor colorFromString(const std::string& text) {
    if (text == "white") return PieceColor::WHITE;
    if (text == "black") return PieceColor::BLACK;
    return PieceColor::None;
}

std::string kindToString(PieceKind kind) {
    switch (kind) {
        case PieceKind::KING: return "king";
        case PieceKind::QUEEN: return "queen";
        case PieceKind::ROOK: return "rook";
        case PieceKind::BISHOP: return "bishop";
        case PieceKind::KNIGHT: return "knight";
        case PieceKind::PAWN: return "pawn";
        default: return "none";
    }
}

PieceKind kindFromString(const std::string& text) {
    if (text == "king") return PieceKind::KING;
    if (text == "queen") return PieceKind::QUEEN;
    if (text == "rook") return PieceKind::ROOK;
    if (text == "bishop") return PieceKind::BISHOP;
    if (text == "knight") return PieceKind::KNIGHT;
    if (text == "pawn") return PieceKind::PAWN;
    return PieceKind::None;
}

std::string stateToString(PieceState state) {
    switch (state) {
        case PieceState::IDLE: return "idle";
        case PieceState::MOVING: return "moving";
        case PieceState::COOLDOWN: return "cooldown";
        case PieceState::CAPTURED: return "captured";
    }
    return "idle";
}

PieceState stateFromString(const std::string& text) {
    if (text == "moving") return PieceState::MOVING;
    if (text == "cooldown") return PieceState::COOLDOWN;
    if (text == "captured") return PieceState::CAPTURED;
    return PieceState::IDLE;
}
}

std::string ServerMessageCodec::typeToString(ServerMessageType type) {
    switch (type) {
        case ServerMessageType::GameSnapshot: return GAME_SNAPSHOT_TYPE;
        case ServerMessageType::RoomCreated: return ROOM_CREATED_TYPE;
        default: throw std::runtime_error("ERROR UNKNOWN_SERVER_MESSAGE_TYPE");
    }
}

ServerMessageType ServerMessageCodec::typeFromString(const std::string& text) {
    if (text == GAME_SNAPSHOT_TYPE) return ServerMessageType::GameSnapshot;
    if (text == ROOM_CREATED_TYPE) return ServerMessageType::RoomCreated;
    throw std::runtime_error("ERROR UNKNOWN_SERVER_MESSAGE_TYPE: " + text);
}

ServerRawMessage ServerMessageCodec::parseRaw(const std::string& text) {
    nlohmann::json data = nlohmann::json::parse(text);
    ServerRawMessage raw;
    raw.type = typeFromString(data.at("type").get<std::string>());
    raw.payload = data.at("payload");
    return raw;
}

std::string ServerMessageCodec::serializeRaw(const ServerRawMessage& message) {
    nlohmann::json data;
    data["type"] = typeToString(message.type);
    data["payload"] = message.payload;
    return data.dump();
}

GameSnapshot ServerMessageCodec::parseGameSnapshot(const ServerRawMessage& raw) {
    GameSnapshot snapshot;
    snapshot.width = raw.payload.at("width").get<int>();
    snapshot.height = raw.payload.at("height").get<int>();
    snapshot.isGameOver = raw.payload.at("isGameOver").get<bool>();
    for (const auto& entry : raw.payload.at("pieces")) {
        MovingPiece piece;
        piece.id = entry.at("id").get<std::string>();
        piece.color = colorFromString(entry.at("color").get<std::string>());
        piece.kind = kindFromString(entry.at("kind").get<std::string>());
        piece.state = stateFromString(entry.at("state").get<std::string>());
        piece.currentPos = ExactPosition{entry.at("row").get<double>(), entry.at("col").get<double>()};
        snapshot.pieces.push_back(piece);
    }
    return snapshot;
}

ServerRawMessage ServerMessageCodec::toRaw(const GameSnapshot& snapshot) {
    ServerRawMessage raw;
    raw.type = ServerMessageType::GameSnapshot;
    raw.payload["width"] = snapshot.width;
    raw.payload["height"] = snapshot.height;
    raw.payload["isGameOver"] = snapshot.isGameOver;
    auto piecesJson = nlohmann::json::array();
    for (const auto& piece : snapshot.pieces) {
        piecesJson.push_back({
            {"id", piece.id},
            {"color", colorToString(piece.color)},
            {"kind", kindToString(piece.kind)},
            {"state", stateToString(piece.state)},
            {"row", piece.currentPos.row},
            {"col", piece.currentPos.col}
        });
    }
    raw.payload["pieces"] = piecesJson;
    return raw;
}

RoomCreatedMessage ServerMessageCodec::parseRoomCreated(const ServerRawMessage& raw) {
    RoomCreatedMessage message;
    message.gameId = raw.payload.at("gameId").get<GameId>();
    ServerRawMessage snapshotRaw{ServerMessageType::GameSnapshot, raw.payload.at("snapshot")};
    message.snapshot = parseGameSnapshot(snapshotRaw);
    return message;
}

ServerRawMessage ServerMessageCodec::toRaw(const RoomCreatedMessage& message) {
    ServerRawMessage raw;
    raw.type = ServerMessageType::RoomCreated;
    raw.payload["gameId"] = message.gameId;
    raw.payload["snapshot"] = toRaw(message.snapshot).payload;
    return raw;
}
