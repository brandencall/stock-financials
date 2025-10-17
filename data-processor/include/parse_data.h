#pragma once
#include "models/company_record.h"
#include "services/company_record_service.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

class DataParser {
  public:
    explicit DataParser(CompanyRecordService &crService, std::unordered_map<std::string, std::string> tagMap);
    void parseAndInsertData(std::filesystem::path path);

  private:
    CompanyRecordService crService;
    std::unordered_map<std::string, std::string> tagMap;
    void parseFinancialFacts(const std::string &cik, const std::string &tag,
                                                   const std::string &friedlyTag, const json &tagData);
    void parseCompanyRecord(const json &entry, CompanyRecord &record);
};
