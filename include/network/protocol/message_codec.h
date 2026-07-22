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

private:
    static std::string typeToString(MessageType type);
    static MessageType typeFromString(const std::string& text);
};
