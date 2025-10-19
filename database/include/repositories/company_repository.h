#pragma once
#include "../database.h"
#include "../models/company.h"
#include <vector>

namespace db::repository {

class CompanyRepository {
  public:
    explicit CompanyRepository(db::Database &db);
    void createTable();
    void upsert(const db::model::Company &company);
    std::vector<db::model::Company> getAll();
    db::model::Company getCompanyByCIK(std::string cik);

  private:
    db::Database &db_;
};

} // namespace db::repository
