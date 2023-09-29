#include "stack_walker.hpp"

namespace FSMonitor
{
void StackWalker::on_thread_done()
{
    std::lock_guard<std::mutex> lock(m_vector_mutex);
    m_free_threads.emplace_back();
}

void StackWalker::worker(path_t const& path)
{
    std::stack<path_t> stack({path});

    updater updater_(std::get<3>(param));
    filter filt = std::get<2>(param);

    do
    {
        path_t const curr_path = stack.top();
        stack.pop();
        for (auto const& dir_entry : std::filesystem::directory_iterator {curr_path, std::filesystem::directory_options::skip_permission_denied})
        {
            if(!filt(dir_entry) || dir_entry.path().filename() == "cache.db" || dir_entry.path().filename() == "cache.db-journal")
                continue;
            updater_.update(dir_entry.path());

            //logging
            //std::cout << dir_entry.path().string() << std::endl;
            //std::cout << m_free_threads.size();

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