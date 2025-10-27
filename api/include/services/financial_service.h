#pragma once

#include "models/company_financials.h"
#include "models/filing.h"
#include "models/financial_fact.h"
#include "models/financial_report.h"
#include "models/stock_price.h"
#include "repositories/company_repository.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include "repositories/stock_price_repository.h"
#include <optional>
#include <vector>

namespace service {

class FinancialService {
  public:
    FinancialService(db::repository::CompanyRepository companyRepo, db::repository::FilingRepository filingRepo,
                     db::repository::FinancialFactRepository factRepo, db::repository::StockPriceRepository stockRepo);

    std::optional<db::model::CompanyFinancials> getByCikAndPeriod(const std::string &cik, const std::string &period,
                                                                  int limit);

  private:
    db::repository::CompanyRepository companyRepo;
    db::repository::FilingRepository filingRepo;
    db::repository::FinancialFactRepository factRepo;
    db::repository::StockPriceRepository stockRepo;

    void addFactPE(std::vector<db::model::FinancialFact> &facts, db::model::StockPrice &stockPrice);
    std::string getEPSCurrency(std::string &epsUnit);
};
} // namespace service
