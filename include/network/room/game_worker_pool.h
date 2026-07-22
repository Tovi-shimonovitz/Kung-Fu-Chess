#pragma once
#include <asio.hpp>
#include <cstddef>
#include <thread>
#include <vector>

class GameWorkerPool {
public:
    explicit GameWorkerPool(std::size_t threadCount = std::thread::hardware_concurrency());
    ~GameWorkerPool();

    GameWorkerPool(const GameWorkerPool&) = delete;
    GameWorkerPool& operator=(const GameWorkerPool&) = delete;

    asio::io_context& ioContext();

private:
    asio::io_context ioContext_;
    asio::executor_work_guard<asio::io_context::executor_type> workGuard_;
    std::vector<std::thread> threads_;
};
