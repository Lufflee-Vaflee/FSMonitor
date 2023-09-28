#pragma once

#include "DB.hpp"
#include "validator_crc32.hpp"

#include <filesystem>
#include <functional>
#include <time.h>
#include <unordered_set>
#include <utility>

namespace FSMonitor
{
class updater
{
   public:

    updater();

    void update(std::filesystem::path const& path);

    ~updater() = default;

   private:
    void update_dir(std::filesystem::path const& path);

    void check_subd_exzistence(std::filesystem::path const& path);

    void update_file(std::filesystem::path const& path);

    DB& db = DB::getInstance();
    std::shared_ptr<DB::executor> exec = db.getExecutor();
};
}// namespace FSMonitor
