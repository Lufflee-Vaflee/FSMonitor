#include <iomanip>
#include <iostream>

#include "sqlite3.h"

#include "filter.hpp"
#include "DB.hpp"
#include "walker.hpp"

int main()
{
    std::cout << "Hello world!" << std::endl;

    FSMonitor::DB& instance = FSMonitor::DB::getInstance();

    while(true);

    return 0;
}