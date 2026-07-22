#pragma once
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <optional>
#include <string>
#include "../protocol/message_codec.h"
#include "../protocol/server_message_codec.h"

class NetworkClient {
public:
    NetworkClient();

    void connect(const std::string& uri);
    void poll();

    void send(const ClientMessage& message);

    std::optional<GameSnapshot> latestSnapshot() const;

private:
    using WsClient = websocketpp::client<websocketpp::config::asio_client>;

    void onOpen(websocketpp::connection_hdl handle);
    void onMessage(websocketpp::connection_hdl handle, WsClient::message_ptr msg);

    WsClient client_;
    websocketpp::connection_hdl handle_;
    bool connected_ = false;
    std::optional<GameSnapshot> latestSnapshot_;
};
