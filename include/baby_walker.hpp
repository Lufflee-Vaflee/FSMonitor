#pragma once
#include <filesystem>
#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>

#include "filter.hpp"
#include "baby_walker.hpp"

namespace FSMonitor
{

class walker 
{
    public:
    walker() = default;

    walker(size_t const thread_count)
        : thread_count(thread_count) { }

    filter filt;
    size_t const thread_count = 3;
    std::atomic<size_t> current_pool = thread_count;
    std::mutex m;

    void operator()(std::filesystem::path path)
    {
        foo(path);

        current_pool++;
    }

    void foo(std::filesystem::path path)
    {
        for(auto const& dir_entry : std::filesystem::directory_iterator(path))
        {
            std::cout << dir_entry.path().string() << std::endl;
            std::cout << current_pool << std::endl;

            if(dir_entry.is_directory())
            {
                m.lock();
                if(current_pool > 1)
                {
                    current_pool--;
                    std::thread also_baby_walker(&walker::operator(), this, dir_entry.path());
                    m.unlock();
                    also_baby_walker.detach();
                }
                else
                {
                    m.unlock();
                    this->foo(dir_entry.path());
                };
            }
        }
    }

};

}