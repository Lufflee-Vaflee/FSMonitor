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

    static DB& getInstance(options_t options = default_options, size_t exec_count = exec_count_default);

    std::shared_ptr<executor> getExecutor();

   private:
    inline static std::filesystem::path const instance = u"cache.db";
    inline static options_t const default_options = SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE;
    inline static size_t const exec_count_default = 3;

    inline static options_t options = default_options;
    std::list<std::shared_ptr<executor>> executors;

    std::mutex _mutex;

    DB(options_t options = default_options, size_t exec_count = exec_count_default);

    DB(DB const& other) = delete;
    DB(DB && other) = delete;
    DB& operator=(DB const& other) = delete;
    DB& operator=(DB const&& other) = delete;

    ~DB() = default;

    void create_db();

   public:
    class executor
    {
        public:

        executor();

        executor(executor const& other) = delete;
        executor& operator=(executor const& other) = delete;
        executor(executor&& other);
        executor& operator=(executor&& other);


        template<int (executor::*callback)(int, char**, char**)>
        static int handler(void* instance, int argc, char** argv, char** azColName);

        int operator()(const char* sql, int (*callback)(void *, int, char **, char **) = nullptr);

        ~executor();

       private:
        sqlite3* connection = nullptr;

        inline bool isValid();
    };
};

}// namespace FSMonitor