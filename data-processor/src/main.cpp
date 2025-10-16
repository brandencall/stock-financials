#include "get_data.h"
#include "models/company.h"
#include "setup.h"
#include <filesystem>
#include <iostream>
#include <vector>

int main() {
    Database db = setup_db();
    CompanyRepository companyRepo = setup_company_repo(db);
    std::filesystem::path companyFactsPath = setup_company_fact_data();
    if (companyFactsPath.empty())
        return 1;

    std::vector<Company> companies = get_sec_company_tickers();
    std::cout << "Inserting into the company table!" << '\n';
    for (const auto &company : companies) {
        companyRepo.upsert(company);
    }
    std::cout << "Done inserting!" << '\n';

    return 0;
}
