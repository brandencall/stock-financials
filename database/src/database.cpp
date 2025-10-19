#include "database.h"
#include <memory>
#include <sqlite_modern_cpp.h>

namespace db {

Database::Database(const std::string &dbPath) : db_(std::make_unique<sqlite::database>(dbPath)) {
    *db_ << "PRAGMA foreign_keys = ON;";
}

sqlite::database &Database::get() { return *db_; }

} // namespace db
