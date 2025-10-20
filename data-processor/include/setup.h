#include "database.h"
#include "get_data.h"
#include "repositories/company_facts_metadata.h"
#include "repositories/company_repository.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

db::Database setup_db();
db::repository::CompanyRepository setup_company_repo(db::Database &db);
db::repository::FilingRepository setup_filing_repo(db::Database &db);
db::repository::FinancialFactRepository setup_fact_repo(db::Database &db);
db::repository::CompanyFactsMetadataRepository setup_company_fact_metadata_repo(db::Database &db);
std::filesystem::path setup_company_fact_data();
std::unordered_map<std::string, std::string> buildTagMap();
