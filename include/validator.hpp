#pragma once

#include <filesystem>

namespace FSMonitor
{

template<typename T>
class validator
{
    public:

    virtual bool operator()(T expected, std::filesystem::path const& path) = 0;

    virtual ~validator() = default;
};

}