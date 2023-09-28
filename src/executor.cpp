#include "DB.hpp"

namespace FSMonitor
{
DB::executor::executor(sqlite3* connection)
{
    this->connection = connection;
    /*if (int error = sqlite3_open_v2(instance.string().c_str(), &connection, options, nullptr))
    {
        if (error != SQLITE_CANTOPEN)
            std::cout << "Eror opening connection cache" << std::endl;
        throw std::exception();
    }
    */
}

DB::executor::executor(executor&& other)
{
    this->connection = other.connection;
    other.connection = nullptr;
}

int DB::executor::operator()(std::string const& stmt, int (*callback)(void*, int, char**, char**), void* data)
{
    if (!isValid())
    {
        std::cout << "Error executor connection is not initialized" << std::endl;
        throw std::exception();
    }

    static unsigned int const MAX_ERR_SIZE = 1024;
    char** errmsg = (char**)sqlite3_malloc(MAX_ERR_SIZE);
    int result = sqlite3_exec(connection, stmt.c_str(), callback, data == nullptr ? _instance : data, errmsg);

    if (result)
    {
        std::cout << "Error occurs while statement execution: " << sqlite3_errstr(result) << std::endl;
        std::cout << *errmsg << std::endl;
        sqlite3_free((void*)errmsg);
        throw std::exception();
    }

    sqlite3_free((void*)errmsg);
    return result;
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

bool DB::executor::isValid()
{
    return this->connection != nullptr;
}

DB::executor::~executor()
{
    if (!isValid())
    {
        // sqlite3_close(connection);
    }
}

}// namespace FSMonitor