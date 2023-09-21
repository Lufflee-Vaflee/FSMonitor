#pragma once

#include "DB.hpp"
#include <filesystem>

namespace FSMonitor
{

class updater
{
    public:
    DB& db = DB::getInstance();
    std::shared_ptr<DB::executor> exec = db.getExecutor();

    int something = 0;

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

    void update_dir(std::filesystem::path const& path)
    {
        std::string stmt = "SELECT * FROM dir WHERE path = \"" + path.string() + "\"";
        (*exec)
        (
            stmt.c_str(),
            DB::executor::handler<updater, &updater::test_handler>
        );

    }

    int test_handler(int argc, char** argv, char** azColName)
    {
        
    }




};

}