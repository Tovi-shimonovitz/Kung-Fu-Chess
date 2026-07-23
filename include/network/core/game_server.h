#pragma once
#include <asio.hpp>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "../connection_info.h"
#include "../registry/connections_registry.h"

class MessageRouter;

class GameServer {
public:
    explicit GameServer(ConnectionsRegistry& registry);

    void setRouter(MessageRouter& router);
    void run(std::uint16_t port);
    void send(ConnectionId id, const std::string& text);
    asio::io_context& ioContext();

private:
    using WsServer = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHandle = websocketpp::connection_hdl;

    WsServer server_;
    ConnectionsRegistry& registry_;
    MessageRouter* router_ = nullptr;

    std::map<ConnectionHandle, ConnectionId, std::owner_less<ConnectionHandle>> handleToId_;
    std::unordered_map<ConnectionId, ConnectionHandle> idToHandle_;
    ConnectionId nextId_ = 1;

    void onOpen(ConnectionHandle handle);
    void onClose(ConnectionHandle handle);
    void onMessage(ConnectionHandle handle, WsServer::message_ptr msg);
};
