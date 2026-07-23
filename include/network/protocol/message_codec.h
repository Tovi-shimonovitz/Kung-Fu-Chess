#pragma once
#include <string>
#include "messages.h"

class MessageCodec {
public:
    static RawMessage parseRaw(const std::string& text);
    static std::string serializeRaw(const RawMessage& message);

    static RegisterMessage parseRegister(const RawMessage& raw);
    static RawMessage toRaw(const RegisterMessage& message);

    static PlayRequestMessage parsePlayRequest(const RawMessage& raw);
    static RawMessage toRaw(const PlayRequestMessage& message);

    static MoveRequestMessage parseMoveRequest(const RawMessage& raw);
    static RawMessage toRaw(const MoveRequestMessage& message);

    static CreateRoomMessage parseCreateRoom(const RawMessage& raw);
    static RawMessage toRaw(const CreateRoomMessage& message);

    static JoinRoomMessage parseJoinRoom(const RawMessage& raw);
    static RawMessage toRaw(const JoinRoomMessage& message);

private:
    static std::string typeToString(MessageType type);
    static MessageType typeFromString(const std::string& text);
};
