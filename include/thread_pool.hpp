//
// Created by JellyfishKnight on 25-3-2.
//

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <ostream>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

namespace utils {

enum class TaskStatus : uint8_t { WAITING = 0, RUNNING, FINISHED };

enum class WorkerStatus : uint8_t { IDLE = 0, RUNNING };

class ThreadPool {
private:
    struct Worker {
        std::thread worker_thread;
        WorkerStatus status = WorkerStatus::IDLE;

        void join() {
            worker_thread.join();
        }
    };

public:
    explicit ThreadPool(std::size_t nums_threads) {
        auto worker_func = [this] {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(this->m_queue_mutex);
                    this->m_condition.wait(lock, [this] { return this->m_stop || !this->m_tasks.empty(); });
                    if (this->m_stop || this->m_tasks.empty()) {
                        return;
                    }
                    task = std::move(m_tasks.front());
                    this->m_tasks.pop_front();
                }
                task();
            }
        };
        for (std::size_t i = 0; i < nums_threads; i++) {
            m_workers.emplace_back(Worker {
                .worker_thread = std::thread(worker_func),
                .status = WorkerStatus::IDLE,
            });
        }
    }

    template<typename F, typename... Args>
    void submit(F&& f, Args&&... args) {
        using ReturnType = void;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<ReturnType> result = task->get_future();

        {
            Task t([task] { (*task)(); });
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            if (m_stop) {
                return ;
            }
            m_tasks.push_back(std::move(t));
        }

        m_condition.notify_one();
        return;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_stop = true;
        }
        m_condition.notify_all();

        for (auto& worker: m_workers) {
            worker.join();
        }
    }

    void stop_now() {
        m_stop = true;
        m_condition.notify_all();

        for (auto& worker: m_workers) {
            worker.join();
        }
    }

    // it won't take effect immediately because this will wait for enough workers to finish their current task
    void resize_worker(std::size_t nums_threads) {
        // lock has been got by sub function in "add worker" and "delete worker"
        if (nums_threads > m_workers.size()) {
            add_worker(nums_threads - m_workers.size());
        } else if (nums_threads < m_workers.size()) {
            delete_worker(m_workers.size() - nums_threads);
        }
    }

    void add_worker(std::size_t nums_threads) {
        auto worker_func = [this] {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(this->m_queue_mutex);
                    this->m_condition.wait(lock, [this] { return this->m_stop || !this->m_tasks.empty(); });
                    if (this->m_stop || this->m_tasks.empty()) {
                        return;
                    }
                    task = std::move(m_tasks.front());
                    this->m_tasks.pop_front();
                }
                task();
            }
        };
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        for (std::size_t i = 0; i < nums_threads; i++) {
            m_workers.emplace_back(Worker {
                .worker_thread = std::thread(worker_func),
                .status = WorkerStatus::IDLE,
            });
        }
    }

    void delete_worker(std::size_t nums_threads) {
        if (m_workers.empty()) {
            return;
        }
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        for (std::size_t i = 0; i < nums_threads; i++) {
            m_workers.back().join();
            m_workers.pop_back();
        }
    }

    [[nodiscard]] bool is_running() const {
        return !m_stop;
    }

    ~ThreadPool() {
        if (!this->m_stop) {
            this->stop();
        }
    }

    std::size_t worker_num() const {
        return m_workers.size();
    }

    /**
     * @brief get the number of tasks in the idle queue
     */
    std::size_t task_num() const {
        return m_tasks.size();
    }

    using SharedPtr = std::shared_ptr<ThreadPool>;
    using UniquePtr = std::unique_ptr<ThreadPool>;

private:
    using Task = std::function<void(void)>;

    std::vector<Worker> m_workers;
    std::deque<Task> m_tasks;

    std::mutex m_queue_mutex;
    std::condition_variable m_condition;

    bool m_stop = false;
};

} // namespace utils

#endif //THREAD_POOL_HPP
