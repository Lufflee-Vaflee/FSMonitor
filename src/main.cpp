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

<<<<<<< Updated upstream
    //FSMonitor::DB& instance = FSMonitor::DB::getInstance();

    FSMonitor::updater upd = FSMonitor::updater();

    upd.check_subd_exzistence("/home");

    while (true)
    {
=======
    std::shared_ptr<FSMonitor::walker> walk = std::make_shared<FSMonitor::StackWalker>(); 
    
    std::filesystem::path const& base = std::filesystem::path("/home");

    while (true)
    {
        (*walk)(base);
>>>>>>> Stashed changes
    }

    return 0;
}