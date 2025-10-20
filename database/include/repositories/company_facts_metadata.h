#pragma once
#include "../database.h"
#include "../models/company_fact_metadata.h"
#include <optional>
#include <string>
#include <vector>

namespace db::repository {

class CompanyFactsMetadataRepository {
  public:
    explicit CompanyFactsMetadataRepository(db::Database &db);
    void createTable();
    void upsert(std::string cik, std::string hash);
    void upsert(model::CompanyFactMetadata metadata);
    std::optional<std::string> getHashByCIK(const std::string &cik);

  private:
    db::Database &db_;
};

} // namespace db::repository
