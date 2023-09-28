#include "DB.hpp"
#include "filter.hpp"
#include "sqlite3.h"
#include "stack_walker.hpp"
#include "updater.hpp"

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Hello world!" << std::endl;

    std::shared_ptr<FSMonitor::walker> walk = std::make_shared<FSMonitor::StackWalker>();

    std::filesystem::path const& base = std::filesystem::path("/home/alexey/spovm");

    while (true)
    {
        (*walk)(base);
    }

    return 0;
}