#include "filter.hpp"

namespace FSMonitor
{

filter::flags filter::convert(std::filesystem::file_type t)
{
    using namespace std::filesystem;

    return t == file_type::not_found ? flags::not_found : static_cast<flags>(pow(2.0, static_cast<double>(t)));
}

std::filesystem::file_type filter::convert(flags t)
{
    using namespace std::filesystem;

    return t == flags::not_found ? file_type::not_found : static_cast<file_type>(std::log2(static_cast<double>(t)));
}

bool filter::isCorresponding(std::filesystem::file_type type, flags to)
{
    using namespace std::filesystem;

    return (to | ~static_cast<int>(convert(type))) == to;
}

filter::filter(std::regex mask, flags allowed)
{
    this->mask = mask;
    this->allowed = allowed;
}

bool filter::isAllowed(std::filesystem::file_type type)
{
    return filter::isCorresponding(type, this->allowed);
}

}