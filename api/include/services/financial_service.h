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
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace service {

class FinancialService {
  public:
    FinancialService(db::repository::CompanyRepository companyRepo, db::repository::FilingRepository filingRepo,
                     db::repository::FinancialFactRepository factRepo, db::repository::StockPriceRepository stockRepo);

    std::optional<db::model::CompanyFinancials> getAllByCikAndPeriod(const std::string &cik, const std::string &period,
                                                                     std::vector<std::string> &facts);
    std::optional<db::model::CompanyFinancials> getByCikAndPeriod(const std::string &cik, const std::string &period,
                                                                  int limit, std::vector<std::string> &facts);

  private:
    db::repository::CompanyRepository companyRepo;
    db::repository::FilingRepository filingRepo;
    db::repository::FinancialFactRepository factRepo;
    db::repository::StockPriceRepository stockRepo;
    std::unordered_map<std::string, std::string> factMap;

    std::optional<db::model::CompanyFinancials> getCompanyFinancials(const std::string &cik, const std::string &period,
                                                                     std::vector<std::string> &facts);

    void normalizeFacts(std::vector<std::string> &facts);
    std::vector<db::model::FinancialReport> getFinancialReports(std::vector<db::model::Filing> filings,
                                                                std::string period, std::vector<std::string> &facts);

    bool filingIsAmendment(const db::model::Filing &filing);

    db::model::FinancialReport *findAssociatedFinancialReport(std::vector<db::model::FinancialReport> &reports,
                                                              const db::model::Filing &filing);

    void updateFinancialReport(db::model::FinancialReport &reportToUpdate,
                               const db::model::FinancialReport &amendmentReport);

    void
    sortByFilingDate(std::unordered_map<std::string, std::vector<db::model::FinancialReport>> &reportsGroupedByYear);

    void addFactPE(std::vector<db::model::FinancialFact> &facts, db::model::StockPrice &stockPrice);
    std::string getEPSCurrency(std::string &epsUnit);
    // Price to book. book = (total assets - total liabilities - intangible)/ shares
    // intangible is Goodwill and IntangibleAssetsNetExcludingGoodwill.
    void addFactTangiblePB(std::vector<db::model::FinancialFact> &facts, db::model::StockPrice &stockPrice);
    // Current Ratio = current assets / current liabilities
    void addFactCurrentRatio(std::vector<db::model::FinancialFact> &facts);
    // Debt to equity ratio = Liabilities / LongTermDebt
    void addFactDebtToEquity(std::vector<db::model::FinancialFact> &facts);
};
} // namespace service
