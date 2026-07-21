#pragma once
#include <nlohmann/json.hpp>
#include "messages.h"

struct RawMessage {
    MessageType type;
    nlohmann::json payload;
};
