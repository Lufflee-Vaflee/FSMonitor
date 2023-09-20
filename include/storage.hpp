#pragma once

#include <exception>
#include <filesystem>
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <functional>
#include <list>

namespace FSMonitor
{
class storage
{
    typedef int options_t;

    private:

    inline static const std::filesystem::path instance = u"cache.db";
    inline static const options_t default_options = SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE;

    sqlite3* connection = nullptr;

    static storage& getInstance(options_t = default_options)
    {
        static auto instance = storage(default_options);

    }

    storage(options_t options = default_options)
    {
        if (auto error = sqlite3_open_v2(instance.string().c_str(), &connection, options, nullptr))
        {
            if (error == SQLITE_CANTOPEN)
            {
                create_db();
            }
            else
            {
                std::cout << "Eror opening connection cache" << std::endl;
                throw std::exception();
            }
        }
    }

    template<int (storage::*callback)(int, char**, char**)>
    static int handler(void* instance, int argc, char** argv, char** azColName)
    {
        return ((storage*)instance->*callback)(argc, argv, azColName);
    }

    inline bool exec(sqlite3* connection, const char *sql, int (*callback)(void *, int, char **, char **) = nullptr)
    {
        static unsigned int const MAX_ERR_SIZE = 1024;
        char** errmsg = (char**)sqlite3_malloc(MAX_ERR_SIZE);
        int result = sqlite3_exec(connection, sql, callback, this, errmsg);

        if(result)
        {
            std::cout << "Error occurs while statement execution: " << sqlite3_errstr(result) << std::endl;
            std::cout << *errmsg << std::endl;
            sqlite3_free((void*)errmsg);
            throw std::exception();
        }

        sqlite3_free((void*)errmsg);
        return result;
    }

    static void create_db()
    {
        if (sqlite3_open_v2(instance.string().c_str(), &connection, default_options | SQLITE_OPEN_CREATE, nullptr))
        {
            std::cout << "Eror opening connection" << std::endl;
            throw std::exception();
        }

        this->exec
        (
            connection,
            "CREATE TABLE dir"
            "("
                "path text NOT NULL, "
                "change_time INTEGER NOT NULL, "
                "CONSTRAINT pk PRIMARY KEY(path, change_time)"
            ");"
        );

        this->exec
        (
            connection,
            "CREATE TABLE file" 
            "("
                "path text NOT NULL, "
                "change_time INTEGER NOT NULL, "
                "crc INTEGER NOT NULL, "
                "CONSTRAINT pk PRIMARY KEY(path, change_time)"
            ");"
        );
    }

    ~storage()
    {
        sqlite3_close(connection);
    }
};

}// namespace FSMonitor