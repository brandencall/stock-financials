#pragma once
#include "../database.h"
#include "../models/company_record.h"
#include "../models/financial_fact.h"
#include <vector>

namespace db::repository {

class FinancialFactRepository {
  public:
    explicit FinancialFactRepository(db::Database &db);
    void createTable();
    void upsert(const db::model::CompanyRecord &record);
    void upsert(const db::model::FinancialFact &financialFact);

  private:
    db::Database &db_;
};

} // namespace db::repository
