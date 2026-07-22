#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include "../../include/network/protocol/message_codec.h"

using WsClient = websocketpp::client<websocketpp::config::asio_client>;

struct TestClient {
    WsClient client;
    websocketpp::connection_hdl hdl;
    std::string name;
    int snapshotCount = 0;

    explicit TestClient(std::string n) : name(std::move(n)) {
        client.clear_access_channels(websocketpp::log::alevel::all);
        client.init_asio();

        client.set_open_handler([this](websocketpp::connection_hdl h) {
            hdl = h;
            std::cout << "[" << name << "] connected" << std::endl;
        });

        client.set_message_handler([this](websocketpp::connection_hdl, WsClient::message_ptr msg) {
            std::string payload = msg->get_payload();
            if (payload.find("game_snapshot") != std::string::npos) {
                snapshotCount++;
                if (snapshotCount == 1) {
                    std::cout << "[" << name << "] first snapshot: " << payload.substr(0, 150) << "..." << std::endl;
                }
            } else {
                std::cout << "[" << name << "] received: " << payload << std::endl;
            }
        });
    }

    void connect(const std::string& uri) {
        websocketpp::lib::error_code ec;
        WsClient::connection_ptr con = client.get_connection(uri, ec);
        if (ec) {
            std::cout << "[" << name << "] connect error: " << ec.message() << std::endl;
            return;
        }
        client.connect(con);
    }

    void send(const std::string& text) {
        client.send(hdl, text, websocketpp::frame::opcode::text);
    }

    void poll() {
        client.get_io_service().poll();
    }
};

void pump(TestClient& a, TestClient& b, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        a.poll();
        b.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    TestClient a("A-white");
    TestClient b("B-black");

    a.connect("ws://localhost:9002");
    b.connect("ws://localhost:9002");
    pump(a, b, 20);

    a.send(MessageCodec::serializeRaw(MessageCodec::toRaw(RegisterMessage{"alice", "pw"})));
    b.send(MessageCodec::serializeRaw(MessageCodec::toRaw(RegisterMessage{"bob", "pw"})));
    pump(a, b, 5);

    std::cout << "--- sending PlayRequest for both ---" << std::endl;
    a.send(MessageCodec::serializeRaw(MessageCodec::toRaw(PlayRequestMessage{})));
    b.send(MessageCodec::serializeRaw(MessageCodec::toRaw(PlayRequestMessage{})));

    std::cout << "--- waiting for snapshot broadcasts ---" << std::endl;
    pump(a, b, 40);

    std::cout << "[A] snapshots so far: " << a.snapshotCount << std::endl;
    std::cout << "[B] snapshots so far: " << b.snapshotCount << std::endl;

    std::cout << "--- sending a legal white move (6,0)->(5,0) ---" << std::endl;
    a.send(MessageCodec::serializeRaw(MessageCodec::toRaw(MoveRequestMessage{Position(6, 0), Position(5, 0)})));
    pump(a, b, 40);

    std::cout << "[A] snapshots after move: " << a.snapshotCount << std::endl;
    std::cout << "[B] snapshots after move: " << b.snapshotCount << std::endl;

    return 0;
}
