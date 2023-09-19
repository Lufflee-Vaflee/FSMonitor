#include <iomanip>
#include <iostream>

#include "sqlite3.h"

#include "filter.hpp"
#include "storage.hpp"
#include "baby_walker.hpp"

int main()
{
    std::cout << "Hello world!" << std::endl;

    auto baby_walker = FSMonitor::walker();

    baby_walker("/home");

    while(true);

    return 0;
}