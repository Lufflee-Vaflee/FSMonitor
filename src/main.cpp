#include "DB.hpp"
#include "filter.hpp"
#include "sqlite3.h"
#include "walker.hpp"

#include <iomanip>
#include <iostream>

int main()
{
    std::cout << "Hello world!" << std::endl;

    FSMonitor::DB& instance = FSMonitor::DB::getInstance();

    while (true);

    return 0;
}