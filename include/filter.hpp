#pragma once

#include "math.h"

#include <filesystem>
#include <regex>
#include <string>
#include <vector>

namespace FSMonitor
{
class filter
{
   public:
    enum flags
    {
        none = 1,
        regular = 2,
        directory = 4,
        symlink = 8,
        block = 16,
        character = 32,
        fifo = 64,
        socket = 128,
        unknown = 256,

        not_found = 512,

        file = none | regular | not_found | character | fifo | socket | unknown,
        all = file | directory,
        nothing = 0
    };

    filter() {}
    explicit filter(std::regex mask, flags allowed = flags::all);

   private:
    flags allowed = flags::all;
    std::regex mask;

   public:
    bool operator()(std::filesystem::directory_entry const& entry)
    {
        return !(std::filesystem::is_directory(entry.path()) && std::regex_match(entry.path().filename().string(), mask) && isAllowed(entry.status().type()));
    }

    static flags convert(std::filesystem::file_type t);

    static std::filesystem::file_type convert(flags t);

    static bool isCorresponding(std::filesystem::file_type type, flags to);

    bool isAllowed(std::filesystem::file_type type);
};

}// namespace FSMonitor