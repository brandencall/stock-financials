#include "services/financial_service.h"

namespace service {

FinancialService::FinancialService(db::repository::CompanyRepository companyRepo,
                                   db::repository::FilingRepository filingRepo,
                                   db::repository::FinancialFactRepository factRepo,
                                   db::repository::StockPriceRepository stockRepo)
    : companyRepo(companyRepo), filingRepo(filingRepo), factRepo(factRepo), stockRepo(stockRepo) {};

std::optional<db::model::CompanyFinancials> FinancialService::getByCikAndPeriod(const std::string &cik,
                                                                                const std::string &period, int limit) {
    db::model::CompanyFinancials result;
    std::optional<db::model::Company> company = companyRepo.getCompanyByCIK(cik);
    if (company == std::nullopt) {
        std::cout << "The company is nullopt for its financials" << '\n';
        return std::nullopt;
    }
    result.company = company.value();

    std::vector<db::model::Filing> filings;
    if (period == "annual") {
        filings = filingRepo.getAnnualFilingsForCIK(cik, limit);
    } else {
        filings = filingRepo.getQuarterlyFilingsForCIK(cik, limit);
    }

    std::vector<db::model::FinancialReport> financialReports;
    for (const auto &filing : filings) {
        db::model::FinancialReport financialReport;
        financialReport.filing = filing;
        financialReport.facts = factRepo.getFilteredByFilingId(filing.filingId);
        std::optional<db::model::StockPrice> stockPrice = stockRepo.getByFilingId(filing.filingId);
        if (stockPrice.has_value()) {
            financialReport.stockPrice = stockPrice.value();
            addFactPE(financialReport.facts, financialReport.stockPrice);
        }
        financialReports.push_back(financialReport);
    }
    result.reports = financialReports;
    return result;
}

//  P/E ratio is calculated with diluted EPS
void FinancialService::addFactPE(std::vector<db::model::FinancialFact> &facts, db::model::StockPrice &stockPrice) {
    std::optional<db::model::FinancialFact> epsFact = std::nullopt;
    for (const auto &f : facts) {
        if (f.sourceTag == "EarningsPerShareDiluted") {
            epsFact = f;
            break;
        }
    }
    if (epsFact.has_value()) {
        std::string epsCurrency = getEPSCurrency(epsFact->unit);
        if (epsCurrency != stockPrice.currency)
            return;
        double eps = epsFact->value;
        double pe = stockPrice.close / eps;
        db::model::FinancialFact peFact;
        peFact.filingId = epsFact->filingId;
        peFact.endDate = epsFact->endDate;
        peFact.value = pe;
        peFact.tag = "PE ratio";
        facts.push_back(peFact);
    }
}

std::string FinancialService::getEPSCurrency(std::string &epsUnit) {
    std::string result;
    char target = '/';
    size_t foundPos = epsUnit.find(target);

    if (foundPos != std::string::npos) {
        result = epsUnit.substr(0, foundPos);
    }

    return result;
}

} // namespace service
