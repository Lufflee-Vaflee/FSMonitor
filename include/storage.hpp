#pragma once

#include <exception>
#include <filesystem>
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <functional>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>

namespace FSMonitor
{
class DB
{
   public:
    typedef int options_t;

    class executor;

    static DB& getInstance(options_t options = default_options, size_t exec_count = exec_count_default)
    {
        static auto instance = DB(options, exec_count);
        return instance;
    }

    std::shared_ptr<executor> getExecutor()
    {
        using namespace std::chrono_literals;

        std::unique_lock<std::mutex>{_mutex};

        int count = 10;
        while(count)
        {
            for(auto i = executors.begin(); i != executors.end(); i++)
            {
                if(i->use_count() == 1)
                    return *i;
            }

            std::this_thread::sleep_for(128ms);
            count--;
        }

        std::cout << "Cant find free connections" << std::endl;
        throw std::exception();
    }

   private:
    inline static std::filesystem::path const instance = u"cache.db";
    inline static options_t const default_options = SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE;
    inline static size_t const exec_count_default = 3;

    inline static options_t options = default_options;
    std::list<std::shared_ptr<executor>> executors;

    std::mutex _mutex;

    DB(options_t options = default_options, size_t exec_count = exec_count_default)
    {
        DB::options = options;
        try
        {
            auto exec = executor();
        }
        catch(const std::exception& e)
        {
            create_db();
        }

        if(exec_count == 0 || exec_count > std::thread::hardware_concurrency())
        {
            std::cout << "Error initializing DB executors too much threads" << std::endl;
            throw std::exception();
        }

        for(size_t i = 0; i < exec_count; i++)
        {
            executors.push_back(std::shared_ptr<executor>(new executor()));
        }
    }

    DB(DB const& other) = delete;
    DB(DB && other) = delete;
    DB& operator=(DB const& other) = delete;
    DB& operator=(DB const&& other) = delete;


    void create_db()
    {
        options = options | SQLITE_OPEN_CREATE;
        auto exec = executor();

        exec
        (
            "CREATE TABLE dir"
            "("
                "path text NOT NULL, "
                "change_time INTEGER NOT NULL, "
                "CONSTRAINT pk PRIMARY KEY(path, change_time)"
            ");"
        );

        exec
        (
            "CREATE TABLE file" 
            "("
                "path text NOT NULL, "
                "change_time INTEGER NOT NULL, "
                "crc INTEGER NOT NULL, "
                "CONSTRAINT pk PRIMARY KEY(path, change_time)"
            ");"
        );
    }

   public:
    class executor
    {
        public:

        executor()
        {
            if (int error = sqlite3_open_v2(instance.string().c_str(), &connection, options, nullptr))
            {
                std::cout << "Eror opening connection cache" << std::endl;
                throw std::exception();
            }
        }

        executor(executor const& other) = delete;
        executor& operator=(executor const& other) = delete;
        executor(executor&& other)
        {
            this->connection = other.connection;
            other.connection = nullptr;
        }
        executor& operator=(executor&& other)
        {
            if(this == &other)
                return *this;

            if(isValid())
                sqlite3_close_v2(connection);

            this->connection = other.connection;
            other.connection = nullptr;

            return *this;
        }


        template<int (executor::*callback)(int, char**, char**)>
        static int handler(void* instance, int argc, char** argv, char** azColName)
        {
            return ((executor*)instance->*callback)(argc, argv, azColName);
        }

        int operator()(const char* sql, int (*callback)(void *, int, char **, char **) = nullptr)
        {
            if(!isValid())
            {
                std::cout << "Error executor connection is not initialized" << std::endl;
                throw std::exception();
            }

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

        ~executor()
        {
            if(!isValid())
            {
                sqlite3_close(connection);
            }
        }

       private:
        sqlite3* connection = nullptr;

        inline bool isValid()
        {
            return connection != nullptr;
        }
    };
};

}// namespace FSMonitor