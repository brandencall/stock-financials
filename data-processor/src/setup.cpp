#include "setup.h"
#include "database.h"
#include "repositories/company_repository.h"

Database setup_db() {
    std::filesystem::path newDir = "../data";
    std::filesystem::create_directories(newDir);
    std::filesystem::path path = std::filesystem::absolute(newDir) / "stock_data.db";
    return Database(path);
}

CompanyRepository setup_company_repo(Database &db) {
    CompanyRepository companies(db);
    companies.createTable();
    return companies;
}
