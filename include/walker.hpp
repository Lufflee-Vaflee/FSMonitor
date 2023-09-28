#pragma once
#include "filter.hpp"
#include "updater.hpp"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

namespace FSMonitor
{
class walker
{
   public:
    using path_t = std::filesystem::path;

    walker(size_t const thread_count = std::thread::hardware_concurrency());

    void operator()(path_t const path);

    virtual ~walker() = default;

   protected:
    size_t const max_thread_count;
    std::vector<std::thread> m_free_threads;
    std::mutex m_vector_mutex;
    std::mutex common;

   private:
    virtual void worker(path_t const& path) = 0;
};

}// namespace FSMonitor