#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <thread>

#include "filter.hpp"

namespace FSMonitor
{

typedef std::tuple<std::filesystem::path, size_t, filter, bool> parametrs;

class parameters_handler
{
    public:

    inline static std::filesystem::path const base = std::filesystem::path("/home");
    inline static std::regex no_filter = std::regex(".*");
    inline static filter::flags ignored = filter::flags::none;
    inline static filter all = filter(no_filter, ignored);
    inline static size_t default_threads_count = std::thread::hardware_concurrency();
    inline static bool trust_filesystem_flag = false;

    parameters_handler() = default;

    parametrs operator()(int argc, char** argv)
    {
        parametrs result;
        std::get<0>(result) = base;
        std::get<1>(result) = default_threads_count;
        std::get<2>(result) = all;
        std::get<3>(result) = trust_filesystem_flag;

        for(int i = 0; i < argc; i++)
        {
            switch (argv[i][0])
            {
            case 'r':

                break;
            case '-':

                break;

            default:
                break;
            }
        }
    }

    ~parameters_handler() = default;

   private:
    void display_help();
};

}