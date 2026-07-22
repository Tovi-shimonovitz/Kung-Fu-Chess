#include "../../../include/network/room/game_worker_pool.h"
#include <algorithm>

GameWorkerPool::GameWorkerPool(std::size_t threadCount)
    : workGuard_(asio::make_work_guard(ioContext_)) {
    std::size_t count = std::max<std::size_t>(1, threadCount);
    threads_.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        threads_.emplace_back([this] { ioContext_.run(); });
    }
}

GameWorkerPool::~GameWorkerPool() {
    workGuard_.reset();
    ioContext_.stop();
    for (auto& t : threads_) {
        if (t.joinable()) t.join();
    }
}

asio::io_context& GameWorkerPool::ioContext() {
    return ioContext_;
}
