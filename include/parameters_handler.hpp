#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <thread>
#include <iostream>

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
    inline static size_t default_threads_count = std::thread::hardware_concurrency();
    inline static bool trust_filesystem_flag = false;

    parameters_handler() = default;

    parametrs operator()(int argc, char** argv);

    ~parameters_handler() = default;

   private:

    std::regex mask = no_filter;
    filter::flags restr_types = ignored;
    std::filesystem::path path = base;
    bool trust_flag = false;
    size_t thread_count = std::thread::hardware_concurrency();

    void parse_flags(int argc, char** argv, int begin);
    void display_help();
};

}