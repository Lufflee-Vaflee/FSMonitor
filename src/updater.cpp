#include "updater.hpp"

namespace FSMonitor
{
updater::updater(bool trust_filesystem)
{
    this->trust_filesystem = trust_filesystem;
    exec->set_instance(this);
}

void updater::update(std::filesystem::path const& path)
{
    if (std::filesystem::is_directory(path))
    {
        update_dir(path);
    }
    else
    {
        update_file(path);
    }
}

void updater::update_dir(std::filesystem::path const& path)
{
    std::string stmt = "SELECT path FROM dir WHERE path = \"" + path.string() + "\" AND delete_time IS NULL;";

    bool exzist;
    (*exec)(
        stmt.c_str(),
        +[](void* exzist, int argc, char** data, char** columns) -> int {
            (void)data;
            (void)columns;
            if (argc == 1)
                *(reinterpret_cast<bool*>(exzist)) = true;
            return 0;
        },
        &exzist);

    if (!exzist)
    {
        time_t t;
        time(&t);
        std::string time = std::to_string(t);
        std::string stmt = "INSERT INTO dir VALUES (\"" + path.string() + "\", " + time + ", NULL);";

        (*exec)(stmt.c_str());
    }
    else
    {
        check_subd_exzistence(path);
    }
}

void updater::check_subd_exzistence(std::filesystem::path const& path)
{
    std::string stmt = "SELECT path FROM dir WHERE path = \"" + path.string() + "/%\" AND path NOT LIKE\"" + path.string() + "/%/%\" AND delete_time IS NULL;";

    //считываем состояние прямых подкаталогов
    std::unordered_set<std::string> catalogs;
    (*exec)(
        stmt.c_str(),
        +[](void* raw, int argc, char** data, char** columns) -> int {
            (void)columns;
            std::unordered_set<std::string>& catalogs = *(reinterpret_cast<std::unordered_set<std::string>*>(raw));
            for (int i = 0; i < argc; i++)
            {
                catalogs.emplace(data[i]);
            }

            return 0;
        },
        &catalogs);

    //считываем состояние прямых файлов
    std::unordered_set<std::string> files;
    stmt = "SELECT path FROM file WHERE path = \"" + path.string() + "/%\" AND path NOT LIKE\"" + path.string() + "/%/%\" AND delete_time IS NULL;";
    (*exec)(
        stmt.c_str(),
        +[](void* raw, int argc, char** data, char** columns) -> int {
            (void)columns;
            std::unordered_set<std::string>& files = *(reinterpret_cast<std::unordered_set<std::string>*>(raw));
            for (int i = 0; i < argc; i++)
            {
                files.emplace(data[i]);
            }

            return 0;
        },
        &files);

    //удаляем из считанных файлов и каталогов те, что все еще существуют
    for (auto const& dir_entry : std::filesystem::directory_iterator {path})
    {
        if (dir_entry.is_directory())
            catalogs.erase(dir_entry.path().string());
        else
            files.erase(dir_entry.path().string());
    }

    //"удаляем" из таблиц файлы и подкаталоги, которые уже не существуют
    for (auto it = catalogs.begin(); it != catalogs.end(); it++)
    {
        time_t t;
        time(&t);

        std::string time = std::to_string(t);

        stmt = "UPDATE dir SET delete_time = " + std::to_string(t) + " WHERE path LIKE \"" + (*it) + "/%\";";
        (*exec)(stmt.c_str());

        stmt = "UPDATE file SET delete_time = " + std::to_string(t) + " WHERE path LIKE \"" + (*it) + "/%\";";
        (*exec)(stmt.c_str());
    }

    //"удаляем" из таблиц прямое содержимое каталога
    for (auto it = files.begin(); it != files.end(); it++)
    {
        time_t t;
        time(&t);

        std::string time = std::to_string(t);

        stmt = "UPDATE file SET delete_time = " + std::to_string(t) + " WHERE path = \"" + (*it) + "\";";
        (*exec)(stmt.c_str());
    }
}

void updater::update_file(std::filesystem::path const& path)
{
    std::string stmt = "SELECT crc FROM file WHERE path = \"" + path.string() + "\" AND delete_time IS NULL;";

    bool exzist = false;
    (*exec)(
        stmt.c_str(),
        +[](void* exzist, int argc, char** data, char** columns) -> int {
            (void)data;
            (void)columns;
            if (argc == 1)
            {
                *(reinterpret_cast<bool*>(exzist)) = true;
            }
            else
            {
                *(reinterpret_cast<bool*>(exzist)) = false;
                return -1;
            }

            return 0;
        },
        &exzist);

    validator<crc32_t> const& valid = validator_crc32();
    if (!exzist)
    {
        time_t t;
        time(&t);
        std::string time = std::to_string(t);

        crc32_t result;

        valid(result, path);

        std::string stmt = "INSERT INTO file VALUES (\"" + path.string() + "\", " + time + ", NULL, " + std::to_string(result) + ");";

        (*exec)(stmt.c_str());
    }
    else if(!trust_filesystem)
    {
        std::string stmt = "SELECT change_time FROM file WHERE path = \"" + path.string() + "\" AND delete_time IS NULL;";

        crc32_t expected;
        (*exec)(
            stmt.c_str(),
            +[](void* expected, int argc, char** data, char** columns) -> int {
                (void)columns;
                if (argc != 1)
                {
                    return -1;
                }

                *(reinterpret_cast<crc32_t*>(expected)) = std::stoi(data[0]);
                return 0;
            },
            &expected);

        if (!valid(expected, path))
        {
            time_t t;
            time(&t);

            std::string time = std::to_string(t);

            //std::cout << time << std::endl;
            //std::cout << path.string() << std::endl;

            stmt = "UPDATE file SET delete_time = " + time + " WHERE path = \"" + path.string() + "\";";
            (*exec)(stmt.c_str());

            stmt = "INSERT INTO file VALUES (\"" + path.string() + "\", " + time + ", NULL, " + std::to_string(expected) + ");";
            (*exec)(stmt.c_str());
        }
    }
    else
    {
        std::string stmt = "SELECT crc, change_time FROM file WHERE path = \"" + path.string() + "\" AND delete_time IS NULL;";

        std::pair<crc32_t, time_t> request;
        (*exec)(
            stmt.c_str(),
            +[](void* raw, int argc, char** data, char** columns) -> int {
                (void)columns;
                if (argc != 1)
                {
                    return -1;
                }

                std::pair<crc32_t, time_t>& request = *(reinterpret_cast<std::pair<crc32_t, time_t>*>(raw));
                request.first = std::stoi(data[0]);
                request.second = std::stoi(data[1]);

                return 0;
            },
            &request);

        crc32_t expected = request.first;
        time_t change_time = request.second;

        using namespace std::chrono;
        using namespace std::filesystem; 

        time_t last_write = system_clock::to_time_t(time_point_cast<system_clock::duration>(last_write_time(path) - file_time_type::clock::now() + system_clock::now()));
        
        if(last_write <= change_time)
        {
            return;
        }

        if (!valid(expected, path))
        {
            time_t t;
            time(&t);

            std::string time = std::to_string(t);

            stmt = "UPDATE file SET delete_time = \"" + time + "\" WHERE path = \"" + path.string() + "\";";
            (*exec)(stmt.c_str());

            stmt = "INSERT INTO file VALUES (\"" + path.string() + "\", " + std::to_string(last_write) + ", NULL, " + std::to_string(expected) + ");";
            (*exec)(stmt.c_str());
        }
    }
}
}// namespace FSMonitorsafasdfasdfasdfasdf