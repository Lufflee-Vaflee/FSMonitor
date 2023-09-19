#pragma once

#include <regex>
#include <string>
#include <filesystem>
#include <vector>
#include "math.h"


namespace FSMonitor
{

class filter
{
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
        all = file | directory
    };

   public:
    filter() {}
    filter(std::regex mask, flags allowed = flags::all);

   private:
    flags allowed = flags::all;
    std::regex mask;


   public:
    std::vector<std::filesystem::path> operator()(std::filesystem::directory_entry entry)
    {
        auto it = std::filesystem::directory_iterator(entry.path());

        std::vector<std::filesystem::path> result;
        for(const auto& entry : it)
        {
            if(isAllowed(entry.status().type()) && std::regex_match(entry.path().filename().string(), mask))
            {
                result.push_back(entry.path());
            }
        }

        return result;
    }

    static flags convert(std::filesystem::file_type t);

    static std::filesystem::file_type convert(flags t);

    static bool isCorresponding(std::filesystem::file_type type, flags to);

    bool isAllowed(std::filesystem::file_type);
};

}