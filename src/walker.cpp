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
    worker(path);
    while (m_free_threads.size() != max_thread_count)
    {
        std::this_thread::sleep_for(1ms);
    }
}

}// namespace FSMonitor