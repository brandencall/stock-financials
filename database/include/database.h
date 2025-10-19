#pragma once

#include <memory>
#include <sqlite_modern_cpp.h>
#include <string>

namespace db {

class Database {
  public:
    explicit Database(const std::string &dbPath);
    sqlite::database &get();

  private:
    std::unique_ptr<sqlite::database> db_;
};

} // namespace db
