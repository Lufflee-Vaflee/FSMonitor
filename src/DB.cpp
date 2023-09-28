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
    if (int error = sqlite3_open_v2(instance.string().c_str(), &connection, options, nullptr))
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

    if (exec_count == 0 || exec_count > std::thread::hardware_concurrency())
    {
        std::cout << "Error initializing DB executors too much threads" << std::endl;
        throw std::exception();
    }

    for (size_t i = 0; i < exec_count; i++)
    {
        executors.push_back(std::shared_ptr<executor>(new executor(connection)));
    }
}

void DB::create_db()
{
    options = options | SQLITE_OPEN_CREATE;
    if (int error = sqlite3_open_v2(instance.string().c_str(), &connection, options, nullptr))
    {
        std::cout << "Eror opening connection cache" << std::endl;
        throw std::exception();
    }

    auto exec = executor(connection);
    exec.set_instance(this);

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

}// namespace FSMonitor