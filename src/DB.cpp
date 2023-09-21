#include <DB.hpp>

namespace FSMonitor
{
DB& DB::getInstance(options_t options, size_t exec_count)
{
    static auto instance = DB(options, exec_count);
    return instance;
}

std::shared_ptr<DB::executor> DB::getExecutor()
{
    using namespace std::chrono_literals;

    std::unique_lock<std::mutex> {_mutex};

    int count = 10;
    while (count)
    {
        for (auto i = executors.begin(); i != executors.end(); i++)
        {
            if (i->use_count() == 1)
                return *i;
        }

        std::this_thread::sleep_for(128ms);
        count--;
    }

    std::cout << "Cant find free connections" << std::endl;
    throw std::exception();
}

DB::DB(options_t options, size_t exec_count)
{
    DB::options = options;
    try
    {
        auto exec = executor();
    }
    catch (const std::exception& e)
    {
        create_db();
    }

    if (exec_count == 0 || exec_count > std::thread::hardware_concurrency())
    {
        std::cout << "Error initializing DB executors too much threads" << std::endl;
        throw std::exception();
    }

    for (size_t i = 0; i < exec_count; i++)
    {
        executors.push_back(std::shared_ptr<executor>(new executor()));
    }
}

void DB::create_db()
{
    options = options | SQLITE_OPEN_CREATE;
    auto exec = executor();

    exec("CREATE TABLE dir"
         "("
         "path text NOT NULL, "
         "change_time INTEGER NOT NULL, "
         "delete_time INTEGER, "
         "CONSTRAINT pk PRIMARY KEY(path, change_time)"
         ");");

    exec("CREATE TABLE file"
         "("
         "path text NOT NULL, "
         "change_time INTEGER NOT NULL, "
         "delete_time INTEGER, "
         "crc INTEGER NOT NULL, "
         "CONSTRAINT pk PRIMARY KEY(path, change_time)"
         ");");
}

int DB::executor::operator()(const char* sql, int (*callback)(void*, int, char**, char**))
{
    if (!isValid())
    {
        std::cout << "Error executor connection is not initialized" << std::endl;
        throw std::exception();
    }

    static unsigned int const MAX_ERR_SIZE = 1024;
    char** errmsg = (char**)sqlite3_malloc(MAX_ERR_SIZE);
    int result = sqlite3_exec(connection, sql, callback, this, errmsg);

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

DB::executor::~executor()
{
    if (!isValid())
    {
        sqlite3_close(connection);
    }
}

bool DB::executor::isValid()
{
    return connection != nullptr;
}

}// namespace FSMonitor