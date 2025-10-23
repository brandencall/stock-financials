#pragma once
#include "curl_wrapper.h"
#include "data_parser.h"
#include "database.h"
#include "models/company.h"
#include "models/stock_price.h"
#include "repositories/company_facts_metadata.h"
#include "repositories/company_repository.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include "repositories/stock_price_repository.h"
#include "utils.h"
#include <filesystem>
#include <iostream>
#include <vector>

class Application {
  public:
    explicit Application(const std::string &dbPath);
    void run();

  private:
    CurlWrapper curl;
    db::Database db;
    db::repository::CompanyRepository companyRepo;
    db::repository::FilingRepository filingRepo;
    db::repository::FinancialFactRepository factRepo;
    db::repository::CompanyFactsMetadataRepository metadataRepo;
    db::repository::StockPriceRepository stockPriceRepo;

    void processCompanyTickers();
    std::string initializeCompanyBulkData(const std::filesystem::path &tmpDir);
    void processCompanyData(const std::filesystem::path &factsDir);
    std::unordered_map<std::string, std::string> buildTagMap();
    void insertPriceData(const std::string &cik);
    std::string getCIK(const std::string &cik);
};
