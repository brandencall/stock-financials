#include "database.h"
#include <memory>
#include <sqlite_modern_cpp.h>

Database::Database(const std::string &dbPath) : db_(std::make_unique<sqlite::database>(dbPath)) {}

sqlite::database &Database::get() { return *db_; }
