#pragma once

#include <chrono>
#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <sqlite3.h>
#include <string>
#include <thread>

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
    inline static options_t const default_options = SQLITE_OPEN_READWRITE;
    inline static size_t const exec_count_default = std::thread::hardware_concurrency();

    inline static options_t options = default_options;
    std::list<std::shared_ptr<executor>> executors;

    std::mutex _mutex;

    sqlite3* connection = nullptr;

    DB(options_t options = default_options, size_t exec_count = exec_count_default);

    DB(DB const& other) = delete;
    DB(DB&& other) = delete;
    DB& operator=(DB const& other) = delete;
    DB& operator=(DB const&& other) = delete;

    ~DB() = default;

    void create_db();

   public:
    class executor
    {
       public:
        executor(sqlite3* connection);

        executor(executor const& other) = delete;
        executor& operator=(executor const& other) = delete;
        executor(executor&& other);
        executor& operator=(executor&& other);

<<<<<<< Updated upstream
        int operator()(std::string const& stmt, int (*callback)(void*, int, char**, char**) = nullptr);
=======
        int operator()(std::string const& stmt, int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr);
>>>>>>> Stashed changes

        ~executor();

        template <typename T, int (T::*callback)(int, char**, char**)>
        static int handler(void* instance, int argc, char** argv, char** azColName)
        {
            return ((T*)instance->*callback)(argc, argv, azColName);
        }

        void set_instance(void* instance)
        {
            this->_instance = instance;
        }

       private:

        void* _instance = nullptr;

        sqlite3* connection = nullptr;

        inline bool isValid();
    };
};

}// namespace FSMonitor