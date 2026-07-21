#include "../../../include/network/core/game_server.h"
#include "../../../include/network/protocol/message_codec.h"
#include "../../../include/network/core/message_router.h"
#include <iostream>

GameServer::GameServer(ConnectionsRegistry& registry)
    : registry_(registry) {
    server_.clear_access_channels(websocketpp::log::alevel::all);
    server_.init_asio();

    server_.set_open_handler([this](ConnectionHandle handle) { onOpen(handle); });
    server_.set_close_handler([this](ConnectionHandle handle) { onClose(handle); });
    server_.set_message_handler([this](ConnectionHandle handle, WsServer::message_ptr msg) {
        onMessage(handle, msg);
    });
}

void GameServer::setRouter(MessageRouter& router) {
    router_ = &router;
}

void GameServer::run(std::uint16_t port) {
    server_.listen(port);
    server_.start_accept();
    std::cout << "[GameServer] listening on port " << port << std::endl;
    server_.run();
}

void GameServer::send(ConnectionId id, const std::string& text) {
    server_.get_io_service().post([this, id, text]() {
        auto it = idToHandle_.find(id);
        if (it == idToHandle_.end()) return;
        try {
            server_.send(it->second, text, websocketpp::frame::opcode::text);
        } catch (const std::exception& e) {
            std::cout << "[GameServer] failed to send to connection " << id << ": " << e.what() << std::endl;
        }
    });
}

void GameServer::onOpen(ConnectionHandle handle) {
    ConnectionId id = nextId_++;
    handleToId_[handle] = id;
    idToHandle_[id] = handle;
    registry_.add(id);
    std::cout << "[GameServer] connection " << id << " opened" << std::endl;
}

void GameServer::onClose(ConnectionHandle handle) {
    auto it = handleToId_.find(handle);
    if (it == handleToId_.end()) return;
    ConnectionId id = it->second;
    registry_.remove(id);
    idToHandle_.erase(id);
    handleToId_.erase(it);
    std::cout << "[GameServer] connection " << id << " closed" << std::endl;
}

void GameServer::onMessage(ConnectionHandle handle, WsServer::message_ptr msg) {
    auto it = handleToId_.find(handle);
    if (it == handleToId_.end()) return;
    ConnectionId id = it->second;

    try {
        RawMessage raw = MessageCodec::parseRaw(msg->get_payload());
        auto error = router_->route(id, raw);
        if (error) send(id, *error);
    } catch (const std::exception& e) {
        send(id, std::string("ERROR: ") + e.what());
    }
}
