#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "validator.hpp"


namespace FSMonitor
{

typedef std::uint32_t crc32_t;

class validator_crc32 : virtual public validator<crc32_t>
{
    public:

    virtual bool operator()(crc32_t expected, std::filesystem::path const& path) override final;

    virtual ~validator_crc32() = default;

    private:

    static std::vector<crc32_t> generate_table();

    inline static std::vector<crc32_t> table = generate_table();
};


}