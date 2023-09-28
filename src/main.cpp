#include "DB.hpp"
#include "filter.hpp"
#include "sqlite3.h"
#include "stack_walker.hpp"
#include "updater.hpp"
#include <chrono>

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Hello world!" << std::endl;

    std::shared_ptr<FSMonitor::walker> walk = std::make_shared<FSMonitor::StackWalker>();

    std::filesystem::path const& base = std::filesystem::path("/home/alexey/spovm");

    using namespace std::chrono_literals;

    while (true)
    {
        (*walk)(base);
        std::this_thread::sleep_for(1s);
    }

    return 0;
}