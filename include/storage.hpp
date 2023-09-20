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
   public:
    typedef int options_t;

    inline static const std::filesystem::path instance = u"cache.db";
    inline static const options_t default_options = SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE;

    std::list<char**> errors = std::list<char**>();

    options_t options = default_options;

    sqlite3* connection = nullptr;

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

    inline int handler(int argc, char** argv, char** azColName)
    {
        return 0;
    }

    inline bool exec(sqlite3* connection, const char *sql, int (*callback)(void *, int, char **, char **))
    {
        char** errmsg = (char**)sqlite3_malloc(1024);
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

    void create_db()
    {
        if (sqlite3_open_v2(instance.string().c_str(), &connection, options | SQLITE_OPEN_CREATE, nullptr))
        {
            std::cout << "Eror opening connection" << std::endl;
            throw std::exception();
        }

        bool result = this->exec
        (
            connection,
            "CREATE TABLE dir"
            "("
                "path text not NULL, "
                "change_time INTEGER NOT NULL, "
                "CONSTRAINT pk PRIMARY KEY(path, change_time)"
            ");",
            handler<&storage::handler>
        );
    }

    ~storage()
    {
        sqlite3_close(connection);
    }
};

}// namespace FSMonitor