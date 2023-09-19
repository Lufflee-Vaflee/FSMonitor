#pragma once
#include "filter.hpp"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

namespace FSMonitor
{
using namespace std::chrono;

class walker
{
    public:

    using path_t = std::filesystem::path;

    walker(size_t const thread_count = std::thread::hardware_concurrency());

    void operator()(path_t const path);

    protected:
    size_t const max_thread_count;
    std::vector<std::thread> m_free_threads;
    std::mutex m_vector_mutex;

    private:
    virtual void worker(path_t const& path) = 0;
};

class StackWalker : public walker
{
    public:

    using path_t = std::filesystem::path;

    StackWalker(size_t const thread_count = std::thread::hardware_concurrency()) : walker {thread_count} {}

    private:
    std::mutex m_stack_mutex;

    void on_thread_done();

    virtual void worker(path_t const& path);
};

}// namespace FSMonitor