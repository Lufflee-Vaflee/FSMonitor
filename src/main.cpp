#include "DB.hpp"
#include "filter.hpp"
#include "sqlite3.h"
#include "walker.hpp"
#include "updater.hpp"

#include <iomanip>
#include <iostream>

int main()
{
    std::cout << "Hello world!" << std::endl;

    //FSMonitor::DB& instance = FSMonitor::DB::getInstance();

    FSMonitor::updater upd = FSMonitor::updater();

    upd.check_subd_exzistence("/home");

    while (true)
    {
    }

    return 0;
}