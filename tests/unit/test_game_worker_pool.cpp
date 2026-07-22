#include <atomic>
#include <cassert>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include "../../include/network/room/game_worker_pool.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_pool_executes_posted_work() {
    GameWorkerPool pool(2);
    std::promise<void> done;
    auto future = done.get_future();

    asio::post(pool.ioContext(), [&done] { done.set_value(); });

    auto status = future.wait_for(std::chrono::seconds(2));
    assert(status == std::future_status::ready);
    log_test("test_pool_executes_posted_work");
}

void test_pool_executes_work_posted_after_a_delay() {
    // Proves the work_guard keeps threads alive even when the pool
    // starts out with nothing queued.
    GameWorkerPool pool(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::promise<void> done;
    auto future = done.get_future();
    asio::post(pool.ioContext(), [&done] { done.set_value(); });

    auto status = future.wait_for(std::chrono::seconds(2));
    assert(status == std::future_status::ready);
    log_test("test_pool_executes_work_posted_after_a_delay");
}

void test_pool_runs_many_tasks_across_threads() {
    const int taskCount = 50;
    GameWorkerPool pool(4);
    std::atomic<int> completed{0};
    std::promise<void> allDone;
    auto future = allDone.get_future();

    for (int i = 0; i < taskCount; ++i) {
        asio::post(pool.ioContext(), [&completed, &allDone, taskCount] {
            if (++completed == taskCount) {
                allDone.set_value();
            }
        });
    }

    auto status = future.wait_for(std::chrono::seconds(2));
    assert(status == std::future_status::ready);
    assert(completed == taskCount);
    log_test("test_pool_runs_many_tasks_across_threads");
}

void test_pool_clamps_zero_thread_count_to_one() {
    GameWorkerPool pool(0);
    std::promise<void> done;
    auto future = done.get_future();

    asio::post(pool.ioContext(), [&done] { done.set_value(); });

    auto status = future.wait_for(std::chrono::seconds(2));
    assert(status == std::future_status::ready);
    log_test("test_pool_clamps_zero_thread_count_to_one");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING GAME WORKER POOL UNIT TESTS   " << std::endl;
    std::cout << "========================================" << std::endl;

    test_pool_executes_posted_work();
    test_pool_executes_work_posted_after_a_delay();
    test_pool_runs_many_tasks_across_threads();
    test_pool_clamps_zero_thread_count_to_one();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL GAME WORKER POOL TESTS EXECUTED    " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
