#include <cassert>
#include <iostream>
#include "../../include/network/protocol/message_codec.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_register_message_round_trip() {
    RegisterMessage original{"alice", "s3cret"};
    RawMessage raw = MessageCodec::toRaw(original);
    assert(raw.type == MessageType::Register);

    RegisterMessage parsed = MessageCodec::parseRegister(raw);
    assert(parsed.username == "alice");
    assert(parsed.password == "s3cret");
    log_test("test_register_message_round_trip");
}

void test_play_request_message_round_trip() {
    PlayRequestMessage original{};
    RawMessage raw = MessageCodec::toRaw(original);
    assert(raw.type == MessageType::PlayRequest);

    MessageCodec::parsePlayRequest(raw);
    log_test("test_play_request_message_round_trip");
}

void test_move_request_message_round_trip() {
    MoveRequestMessage original{Position(1, 2), Position(3, 4)};
    RawMessage raw = MessageCodec::toRaw(original);
    assert(raw.type == MessageType::MoveRequest);

    MoveRequestMessage parsed = MessageCodec::parseMoveRequest(raw);
    assert(parsed.from == Position(1, 2));
    assert(parsed.to == Position(3, 4));
    log_test("test_move_request_message_round_trip");
}

void test_raw_message_serializes_to_and_from_json_text() {
    MoveRequestMessage original{Position(0, 0), Position(1, 1)};
    RawMessage raw = MessageCodec::toRaw(original);

    std::string text = MessageCodec::serializeRaw(raw);
    RawMessage roundTripped = MessageCodec::parseRaw(text);

    assert(roundTripped.type == MessageType::MoveRequest);
    MoveRequestMessage parsed = MessageCodec::parseMoveRequest(roundTripped);
    assert(parsed.from == Position(0, 0));
    assert(parsed.to == Position(1, 1));
    log_test("test_raw_message_serializes_to_and_from_json_text");
}

void test_unknown_message_type_throws() {
    bool threw = false;
    try {
        MessageCodec::parseRaw("{\"type\":\"bogus\",\"payload\":{}}");
    } catch (const std::exception&) {
        threw = true;
    }
    assert(threw);
    log_test("test_unknown_message_type_throws");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING MESSAGE CODEC UNIT TESTS      " << std::endl;
    std::cout << "========================================" << std::endl;

    test_register_message_round_trip();
    test_play_request_message_round_trip();
    test_move_request_message_round_trip();
    test_raw_message_serializes_to_and_from_json_text();
    test_unknown_message_type_throws();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL MESSAGE CODEC TESTS EXECUTED       " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
