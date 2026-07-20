#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "connection_info.h"
#include "connections_registry.h"
#include "message_router.h"

class GameServer {
public:
    GameServer(ConnectionsRegistry& registry, MessageRouter& router);

    void run(std::uint16_t port);
    void send(ConnectionId id, const std::string& text);

private:
    using WsServer = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHandle = websocketpp::connection_hdl;

    WsServer server_;
    ConnectionsRegistry& registry_;
    MessageRouter& router_;

    std::map<ConnectionHandle, ConnectionId, std::owner_less<ConnectionHandle>> handleToId_;
    std::unordered_map<ConnectionId, ConnectionHandle> idToHandle_;
    ConnectionId nextId_ = 1;

    void onOpen(ConnectionHandle handle);
    void onClose(ConnectionHandle handle);
    void onMessage(ConnectionHandle handle, WsServer::message_ptr msg);
};
