#include "services/financial_service.h"
#include "models/financial_fact.h"
#include "models/financial_report.h"
#include <iterator>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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
        filings = filingRepo.getAnnualFilingsForCIK(cik);
    } else {
        filings = filingRepo.getQuarterlyFilingsForCIK(cik);
    }

    std::vector<db::model::FinancialReport> reports = getFinancialReports(filings, period);
    if ((int)reports.size() > limit) {
        std::copy(reports.end() - limit, reports.end(), std::back_inserter(result.reports));
    } else {
        result.reports = reports;
    }
    return result;
}

std::vector<db::model::FinancialReport> FinancialService::getFinancialReports(std::vector<db::model::Filing> filings,
                                                                              std::string period) {
    std::vector<db::model::FinancialReport> result;

    for (const auto &filing : filings) {
        db::model::FinancialReport financialReport;
        financialReport.filing = filing;
        financialReport.facts = factRepo.getFilteredByFilingId(filing.filingId, period);
        std::optional<db::model::StockPrice> stockPrice = stockRepo.getByFilingId(filing.filingId);
        if (stockPrice.has_value()) {
            financialReport.stockPrice = stockPrice.value();
            addFactPE(financialReport.facts, financialReport.stockPrice);
        }
        if (filingIsAmendment(filing)) {
            db::model::FinancialReport *foundReport = findAssociatedFinancialReport(result, filing);
            if (foundReport == nullptr) {
                result.push_back(std::move(financialReport));
            } else {
                updateFinancialReport(*foundReport, financialReport);
            }
        } else {
            result.push_back(std::move(financialReport));
        }
    }
    return result;
}

bool FinancialService::filingIsAmendment(const db::model::Filing &filing) {
    std::string form = filing.form;
    if (form.length() > 2) {
        std::string lastTwoChars = form.substr(form.length() - 2);
        if (lastTwoChars == "/A" || lastTwoChars == "/a")
            return true;
    }
    return false;
}

db::model::FinancialReport *
FinancialService::findAssociatedFinancialReport(std::vector<db::model::FinancialReport> &reports,
                                                const db::model::Filing &filing) {
    for (auto &report : reports) {
        if (report.filing.fp == filing.fp && report.filing.fy == filing.fy)
            return &report;
    }
    return nullptr;
}

void FinancialService::updateFinancialReport(db::model::FinancialReport &reportToUpdate,
                                             const db::model::FinancialReport &amendmentReport) {
    for (auto &fact : reportToUpdate.facts) {
        auto amendmentFact = std::find_if(amendmentReport.facts.begin(), amendmentReport.facts.end(),
                                          [&](const db::model::FinancialFact &ff) { return ff.tag == fact.tag; });
        if (amendmentFact != amendmentReport.facts.end()) {
            fact = *amendmentFact;
        }
    }
}

void FinancialService::sortByFilingDate(
    std::unordered_map<std::string, std::vector<db::model::FinancialReport>> &reportsGroupedByYear) {
    for (auto &[year, report] : reportsGroupedByYear) {
        std::sort(report.begin(), report.end(),
                  [](const db::model::FinancialReport &a, const db::model::FinancialReport &b) {
                      return a.filing.filedDate < b.filing.filedDate;
                  });
    }
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
