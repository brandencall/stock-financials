#include "models/financial_report.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include "repositories/stock_price_repository.h"
#include "services/finacial_service.h"

namespace service {

FinancialService::FinancialService(db::repository::CompanyRepository companyRepo,
                                   db::repository::FilingRepository filingRepo,
                                   db::repository::FinancialFactRepository factRepo,
                                   db::repository::StockPriceRepository stockRepo)
    : companyRepo(companyRepo), filingRepo(filingRepo), factRepo(factRepo), stockRepo(stockRepo) {};

std::optional<db::model::CompanyFinancials> FinancialService::getByCikAndPeriod(const std::string &cik,
                                                                                const std::string &period) {
    db::model::CompanyFinancials result;
    std::optional<db::model::Company> company = companyRepo.getCompanyByCIK(cik);
    if (company == std::nullopt) {
        std::cout << "The company is nullopt for its financials" << '\n';
        return std::nullopt;
    }
    result.company = company.value();

    std::vector<db::model::Filing> filings;
    if (period == "annual") {
        filings = filingRepo.getAnnualFilingsForCIK(cik);
    } else {
        filings = filingRepo.getQuarterlyFilingsForCIK(cik);
    }

    std::vector<db::model::FinancialReport> financialReports;
    for (const auto &filing : filings) {
        db::model::FinancialReport financialReport;
        financialReport.filing = filing;
        financialReport.facts = factRepo.getFilteredByFilingId(filing.filingId);
        financialReport.stockPrice = stockRepo.getByFilingId(filing.filingId);
        financialReports.push_back(financialReport);
    }
    result.reports = financialReports;
    return result;
}

} // namespace service
