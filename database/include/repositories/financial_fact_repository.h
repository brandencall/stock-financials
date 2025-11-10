#pragma once
#include "../database.h"
#include "../models/company_record.h"
#include "../models/financial_fact.h"
#include <unordered_set>

namespace db::repository {

class FinancialFactRepository {
  public:
    explicit FinancialFactRepository(db::Database &db);
    void createTable();
    void upsert(const db::model::CompanyRecord &record);
    void upsert(const db::model::FinancialFact &financialFact);
    std::vector<model::FinancialFact> getByFilingId(int filingId);
    std::vector<model::FinancialFact> getAllFilteredByFilingId(int filingId, std::string period);
    std::vector<model::FinancialFact> getFilteredByFilingIdAndFacts(int filingId, std::string period,
                                                                    std::unordered_set<std::string> facts);

  private:
    db::Database &db_;
};

} // namespace db::repository
