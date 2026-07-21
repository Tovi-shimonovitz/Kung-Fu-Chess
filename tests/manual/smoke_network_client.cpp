#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include "../../include/network/protocol/message_codec.h"

using WsClient = websocketpp::client<websocketpp::config::asio_client>;

int main() {
    WsClient client;
    client.clear_access_channels(websocketpp::log::alevel::all);
    client.init_asio();

    client.set_open_handler([&client](websocketpp::connection_hdl hdl) {
        std::cout << "[Client] connected - sending MoveRequest before registering (expect rejection)" << std::endl;
        RawMessage raw = MessageCodec::toRaw(MoveRequestMessage{Position(0, 0), Position(1, 1)});
        client.send(hdl, MessageCodec::serializeRaw(raw), websocketpp::frame::opcode::text);
    });

    client.set_message_handler([&client](websocketpp::connection_hdl hdl, WsClient::message_ptr msg) {
        std::cout << "[Client] received from server: " << msg->get_payload() << std::endl;
        client.close(hdl, websocketpp::close::status::normal, "done");
    });

    client.set_fail_handler([](websocketpp::connection_hdl) {
        std::cout << "[Client] connection failed - is ChessServer running?" << std::endl;
    });

    websocketpp::lib::error_code ec;
    WsClient::connection_ptr con = client.get_connection("ws://localhost:9002", ec);
    if (ec) {
        std::cout << "[Client] could not create connection: " << ec.message() << std::endl;
        return 1;
    }

    client.connect(con);
    client.run();
    return 0;
}
