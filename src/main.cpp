#include <iomanip>
#include <iostream>

#include "sqlite3.h"

#include "filter.hpp"
#include "storage.hpp"
#include "walker.hpp"

int main()
{
    std::cout << "Hello world!" << std::endl;

    auto baby_walker = FSMonitor::StackWalker();

    baby_walker("/home");
    return 0;
}