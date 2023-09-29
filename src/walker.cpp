#include "walker.hpp"

namespace FSMonitor
{
using namespace std::chrono;

walker::walker(parametrs const& param) 
    : max_thread_count {std::get<1>(param)},
      param(param)
{
    m_free_threads.resize(max_thread_count);
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