#include "walker.hpp"

namespace FSMonitor
{
using namespace std::chrono;

walker::walker(size_t const thread_count) : max_thread_count {thread_count}
{
    m_free_threads.resize(thread_count);
}

void walker::operator()(path_t const path)
{
    std::lock_guard<std::mutex> lock(common);
    worker(path);
    while (m_free_threads.size() != max_thread_count)
    {
        std::this_thread::sleep_for(32ms);
    }
}

}// namespace FSMonitor