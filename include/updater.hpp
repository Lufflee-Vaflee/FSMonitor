#pragma once

#include "DB.hpp"
#include <filesystem>
#include <functional>
#include <utility>
#include <set>
#include <time.h>

namespace FSMonitor
{

class updater
{
    public:
    DB& db = DB::getInstance();
    std::shared_ptr<DB::executor> exec = db.getExecutor();

    std::filesystem::path current;

    updater()
    {
        exec->set_instance(this);
    }

    void update(std::filesystem::path const& path)
    {
        if(std::filesystem::is_directory(path))
        {

        }
        else
        {

        }
    }

    std::set<std::string> catalogs;
    void check_subd_exzistence(std::filesystem::path const& path)
    {
        std::string stmt = "SELECT path FROM dir WHERE path = \"" + path.string() + "/%\" AND NOT \"" + path.string() + "/%/%\" AND delete_time = NULL;";

        (*exec)
        (
            stmt.c_str(),
            +[](void* instance, int argc, char** data, char** columns) -> int
            {
                for(int i = 0; i < argc; i++)
                {
                    ((updater*)instance)->catalogs.emplace(data[i]);
                }

                return 0;
            }
        );

        for(auto const& dir_entry : std::filesystem::directory_iterator {path})
        {
            catalogs.erase(dir_entry.path().string());
        }

        stmt.clear();
        for(auto it = catalogs.begin(); it != catalogs.end(); it++)
        {
            time_t t;
            time(&t);

            std::string time = std::to_string(t);

            stmt = "UPDATE dir SET delete_time = \"" + std::to_string(t) + "\" WHERE path = \"" + (*it) + "\";";
            (*exec)
            (
                stmt.c_str()
            );
        }
    }

    void update_dir(std::filesystem::path const& path)
    {
        current = path;
        std::string stmt = "SELECT * FROM dir WHERE path = \"" + path.string() + "\" AND delete_time = NULL;";

        bool exzist;
        (*exec)
        (
            stmt.c_str(),
            +[](void* instance, int argc, char** data, char** columns) -> int
            {
                return 0;
            }
        );
    }

    int find_last_dir(int count, char** data, char** columns)
    {
        
        return 0;
    }


};
}