#pragma once
#include "filter.hpp"
#include "updater.hpp"
#include "walker.hpp"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

namespace FSMonitor
{
using namespace std::chrono;

class StackWalker : public walker
{
   public:
    using path_t = std::filesystem::path;

    StackWalker(parametrs const& param) : walker {param} {}

    virtual ~StackWalker() = default;

   private:
    std::mutex m_stack_mutex;

    void on_thread_done();

    virtual void worker(path_t const& path);
};

}// namespace FSMonitor