#include "DB.hpp"
#include "filter.hpp"
#include "sqlite3.h"
#include "stack_walker.hpp"
#include "updater.hpp"
#include <chrono>
#include "parameters_handler.hpp"

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Hello world!" << std::endl;

    FSMonitor::parameters_handler instance = FSMonitor::parameters_handler();
    FSMonitor::parametrs param = instance(argc, argv);

    std::shared_ptr<FSMonitor::walker> walk = std::make_shared<FSMonitor::StackWalker>(param);

    std::filesystem::path const& base = std::filesystem::path(std::get<0>(param));

    using namespace std::chrono_literals;

    while (true)
    {
        (*walk)(base);
        std::this_thread::sleep_for(1s);
    }

    return 0;
}