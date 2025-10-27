#pragma once
#include "../database.h"
#include "../models/company_record.h"
#include "../models/filing.h"
#include <optional>
#include <vector>

namespace db::repository {

class FilingRepository {
  public:
    explicit FilingRepository(db::Database &db);
    void createTable();
    int upsert(const db::model::CompanyRecord &record);
    int upsert(const db::model::Filing &filing);
    std::optional<int> getFileIdByAccession(std::string &accession);
    std::vector<db::model::Filing> getFilingsForCIK(const std::string &cik);
    std::vector<db::model::Filing> getAnnualFilingsForCIK(const std::string &cik);
    std::vector<db::model::Filing> getQuarterlyFilingsForCIK(const std::string &cik);

  private:
    db::Database &db_;
};

} // namespace db::repository
