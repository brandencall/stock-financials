#include "data_parser.h"
#include "get_data.h"
#include "models/company.h"
#include "repositories/company_repository.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include "setup.h"
#include <filesystem>
#include <iostream>
#include <vector>

int main() {
    db::Database db = setup_db();
    db::repository::CompanyRepository companyRepo = setup_company_repo(db);
    db::repository::FilingRepository filingRepo = setup_filing_repo(db);
    db::repository::FinancialFactRepository factRepo = setup_fact_repo(db);
    // std::filesystem::path companyFactsPath = setup_company_fact_data();
    // if (companyFactsPath.empty())
    //     return 1;

    // std::vector<Company> companies = get_sec_company_tickers();
    // std::cout << "Inserting into the company table!" << '\n';
    // for (const auto &company : companies) {
    //     companyRepo.upsert(company);
    // }
    // std::cout << "Done inserting!" << '\n';
    std::filesystem::path tmp = std::filesystem::absolute("../data/tmp") / "companyfacts/CIK0000320193.json";
    auto tagMap = buildTagMap();

    DataParser parser(tagMap, filingRepo, factRepo);
    parser.parseAndInsertData(tmp);

    return 0;
}
