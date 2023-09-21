#include "DB.hpp"

namespace FSMonitor
{
DB::executor::executor()
{
    if (int error = sqlite3_open_v2(instance.string().c_str(), &connection, options, nullptr))
    {
        if(error != SQLITE_CANTOPEN)
            std::cout << "Eror opening connection cache" << std::endl;
        throw std::exception();
    }
}

DB::executor::executor(executor&& other)
{
    this->connection = other.connection;
    other.connection = nullptr;
}

DB::executor& DB::executor::operator=(executor&& other)
{
    if (this == &other)
        return *this;

    if (isValid())
        sqlite3_close_v2(connection);

    this->connection = other.connection;
    other.connection = nullptr;

    return *this;
}

template <int (DB::executor::*callback)(int, char**, char**)>
static int handler(void* instance, int argc, char** argv, char** azColName)
{
    return ((DB::executor*)instance->*callback)(argc, argv, azColName);
}

}// namespace FSMonitor