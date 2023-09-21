#include "validator_crc32.hpp"

namespace FSMonitor
{
bool validator_crc32::operator()(crc32_t expected, std::filesystem::path const& path)
{
    std::ifstream file(path);

    crc32_t c = 0xFFFFFFFF;

    uint8_t u;
    while (!file.eof())
    {
        file >> u;
        c = table[(c ^ u) & 0xFF] ^ (c >> 8);
    }

    c = c ^ 0xFFFFFFFF;

    return expected == c;
}

std::vector<crc32_t> validator_crc32::generate_table()
{
    auto result = std::vector<crc32_t>(256);
    crc32_t polynomial = 0xEDB88320;
    for (size_t i = 0; i < result.size(); i++)
    {
        unsigned int c = i;
        for (size_t j = 0; j < 8; j++)
        {
            if (c & 1)
            {
                c = polynomial ^ (c >> 1);
            }
            else
            {
                c >>= 1;
            }
        }

        result[i] = c;
    }

    return result;
}

}// namespace FSMonitor
