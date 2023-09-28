#pragma once
#include "filter.hpp"
#include "updater.hpp"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

#include "walker.hpp"

namespace FSMonitor
{
using namespace std::chrono;

class StackWalker : public walker
{
   public:
    using path_t = std::filesystem::path;

    StackWalker(size_t const thread_count = std::thread::hardware_concurrency()) : walker {thread_count} {}

    virtual ~StackWalker() = default;

   private:
    std::mutex m_stack_mutex;

    void on_thread_done();

    virtual void worker(path_t const& path);
};

}// namespace FSMonitor