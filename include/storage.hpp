#pragma once

#include <filesystem>
#include <sqlite3.h>
#include <iostream>
#include <exception>
#include <string>

namespace FSMonitor
{

class storage
{
    public:

    inline static const std::filesystem::path instance = L"dnfuiqplfwue";

    sqlite3* db = nullptr;

    storage()
    {
        if(sqlite3_open16(instance.u16string().c_str(), &db))
        {
            std::cout << "Eror opening db cache" << std::endl;
            throw std::exception();
        }
    }

    void init()
    {
        
    }

    ~storage()
    {
        sqlite3_close(db);
    }

};

}