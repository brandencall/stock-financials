#pragma once
#include "models/company_record.h"
#include "models/filing.h"
#include "models/financial_fact.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include <fstream>
#include <iostream>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

class DataParser {
  public:
    explicit DataParser(std::unordered_map<std::string, std::string> tagMap,
                        db::repository::FilingRepository &filingRepo,
                        db::repository::FinancialFactRepository &factRepo);
    void parseAndInsertData(const std::string &filename);

  private:
    std::unordered_map<std::string, std::string> tagMap;
    db::repository::FilingRepository filingRepo;
    db::repository::FinancialFactRepository factRepo;
    void parseAndInsertTagData(db::model::CompanyRecord &record, const json &tagData,
                               std::unordered_map<std::string, int> &filing_map);
    void parseCompanyRecord(const json &entry, db::model::CompanyRecord &record);
    void handleNonCachedAccession(db::model::CompanyRecord &record, std::unordered_map<std::string, int> &filing_map);
};
