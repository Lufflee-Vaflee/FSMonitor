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

void StackWalker::on_thread_done()
{
    std::lock_guard<std::mutex> lock(m_vector_mutex);
    m_free_threads.emplace_back();
}

void StackWalker::worker(path_t const& path)
{
    std::stack<path_t> stack({path});

    updater updater_;

    do
    {
        path_t const curr_path = stack.top();
        stack.pop();
        for (auto const& dir_entry : std::filesystem::directory_iterator {curr_path, std::filesystem::directory_options::skip_permission_denied})
        {
            updater_.update(dir_entry.path());
            std::cout << dir_entry.path().string() << std::endl;
            std::cout << m_free_threads.size();
            if (dir_entry.is_directory())
            {
                m_vector_mutex.lock();
                if (m_free_threads.size() == 0)
                {
                    m_vector_mutex.unlock();
                    stack.push(dir_entry);
                }
                else
                {
                    auto& t = m_free_threads.back() = std::thread([dir_entry, this]() {
                        this->worker(dir_entry);
                        this->on_thread_done();
                    });
                    t.detach();
                    m_free_threads.pop_back();
                    m_vector_mutex.unlock();
                }
            }
        }

    } while (!stack.empty());
}

}// namespace FSMonitor