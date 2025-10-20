// application.h
#pragma once
#include "database.h"
#include "get_data.h"
#include "data_parser.h"
#include "repositories/company_facts_metadata.h"
#include "repositories/company_repository.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include "utils.h"
#include <filesystem>

class Application {
  public:
    explicit Application(const std::string &dbPath);
    void run();

  private:
    db::Database db;
    db::repository::CompanyRepository companyRepo;
    db::repository::FilingRepository filingRepo;
    db::repository::FinancialFactRepository factRepo;
    db::repository::CompanyFactsMetadataRepository metadataRepo;

    void processCompanyTickers();
    std::string initializeCompanyBulkData();
    void processCompanyFacts(const std::filesystem::path &factsDir);
    std::unordered_map<std::string, std::string> buildTagMap();
    std::string getCIK(std::string cik);
};
