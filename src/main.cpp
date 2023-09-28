#include "DB.hpp"
#include "filter.hpp"
#include "sqlite3.h"
#include "stack_walker.hpp"
#include "updater.hpp"

#include <iomanip>
#include <iostream>

int main()
{
    std::cout << "Hello world!" << std::endl;

    std::shared_ptr<FSMonitor::walker> walk = std::make_shared<FSMonitor::StackWalker>(); 
    
    std::filesystem::path const& base = std::filesystem::path("/home");

    while (true)
    {
        (*walk)(base);
    }

    return 0;
}