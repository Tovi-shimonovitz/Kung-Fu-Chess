#include "../../../include/network/client/network_client.h"
#include <iostream>

NetworkClient::NetworkClient() {
    client_.clear_access_channels(websocketpp::log::alevel::all);
    client_.init_asio();
    client_.set_open_handler([this](websocketpp::connection_hdl h) { onOpen(h); });
    client_.set_close_handler([this](websocketpp::connection_hdl) { connected_ = false; });
    client_.set_message_handler([this](websocketpp::connection_hdl h, WsClient::message_ptr msg) {
        onMessage(h, msg);
    });
}

void NetworkClient::connect(const std::string& uri) {
    websocketpp::lib::error_code ec;
    WsClient::connection_ptr con = client_.get_connection(uri, ec);
    if (ec) {
        std::cout << "[NetworkClient] connect error: " << ec.message() << std::endl;
        return;
    }
    client_.connect(con);
}

void NetworkClient::poll() {
    client_.get_io_service().poll();
}

void NetworkClient::onOpen(websocketpp::connection_hdl handle) {
    handle_ = handle;
    connected_ = true;
    std::cout << "[NetworkClient] connected" << std::endl;
}

void NetworkClient::send(const ClientMessage& message) {
    if (!connected_) return;
    RawMessage raw = message.toRaw();
    client_.send(handle_, MessageCodec::serializeRaw(raw), websocketpp::frame::opcode::text);
}

std::optional<GameSnapshot> NetworkClient::latestSnapshot() const {
    return latestSnapshot_;
}

void NetworkClient::onMessage(websocketpp::connection_hdl, WsClient::message_ptr msg) {
    std::string payload = msg->get_payload();
    try {
        ServerRawMessage raw = ServerMessageCodec::parseRaw(payload);
        if (raw.type == ServerMessageType::GameSnapshot) {
            latestSnapshot_ = ServerMessageCodec::parseGameSnapshot(raw);
        }
    } catch (const std::exception&) {
        std::cout << "[NetworkClient] server message: " << payload << std::endl;
    }
}
