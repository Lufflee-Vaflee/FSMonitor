#pragma once

#include "DB.hpp"
#include "validator_crc32.hpp"

#include <filesystem>
#include <functional>
#include <time.h>
#include <unordered_set>
#include <utility>
#include <chrono>

namespace FSMonitor
{
class updater
{
   public:
    updater(bool trust_filesystem = false);

    void update(std::filesystem::path const& path);

    ~updater() = default;

   private:
    void update_dir(std::filesystem::path const& path);

    void check_subd_exzistence(std::filesystem::path const& path);

    void update_file(std::filesystem::path const& path);

    DB& db = DB::getInstance();
    std::shared_ptr<DB::executor> exec = db.getExecutor();

    bool trust_filesystem;
};
}// namespace FSMonitor
