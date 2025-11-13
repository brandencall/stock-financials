#include "services/financial_service.h"
#include "models/financial_fact.h"
#include "models/financial_report.h"
#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace service {

FinancialService::FinancialService(db::repository::CompanyRepository companyRepo,
                                   db::repository::FilingRepository filingRepo,
                                   db::repository::FinancialFactRepository factRepo,
                                   db::repository::StockPriceRepository stockRepo)
    : companyRepo(companyRepo), filingRepo(filingRepo), factRepo(factRepo), stockRepo(stockRepo) {

    factMap = {{"revenue", {"Revenue"}},
               {"gross profit", {"Gross Profit"}},
               {"operating income", {"Operating Income"}},
               {"net income", {"Net Income"}},
               {"earnings per share basic", {"Earnings Per Share Basic"}},
               {"earnings per share diluted", {"Earnings Per Share Diluted"}},
               {"cash and cash equivalents", {"Cash And Cash Equivalents"}},
               {"accounts receivable", {"Accounts Receivable"}},
               {"cash flow", {"Cash Flow"}},
               {"overall debt", {"Overall Debt"}},
               {"current debt", {"Current Debt"}},
               {"long term debt", {"Long Term Debt"}},
               {"commercial paper", {"Commercial Paper"}},
               {"stockholders' equity", {"Stockholders' Equity"}},
               {"shares", {"Shares"}},
               {"total assets", {"Total Assets"}},
               {"current assets", {"Current Assets"}},
               {"inventory", {"Inventory"}},
               {"total liabilities", {"Total Liabilities"}},
               {"current liabilities", {"Current Liabilities"}},
               {"capital expenditures", {"Capital Expenditures"}},
               {"goodwill", {"Goodwill"}},
               {"intangible assets", {"Intangible Assets"}},
               {"dividends per share", {"Dividends Per Share"}},
               {"pe ratio", {"Earnings Per Share Diluted"}}};
};

std::optional<db::model::CompanyFinancials> FinancialService::getAllByCikAndPeriod(const std::string &cik,
                                                                                   const std::string &period,
                                                                                   std::vector<std::string> &facts) {
    return getCompanyFinancials(cik, period, facts);
}

std::optional<db::model::CompanyFinancials> FinancialService::getByCikAndPeriod(const std::string &cik,
                                                                                const std::string &period, int limit,
                                                                                std::vector<std::string> &facts) {
    std::optional<db::model::CompanyFinancials> result = getCompanyFinancials(cik, period, facts);
    if (!result) {
        return std::nullopt;
    }

    std::vector<db::model::FinancialReport> &reports = result->reports;
    if (reports.size() > static_cast<size_t>(limit)) {
        result->reports = {reports.end() - limit, reports.end()};
    }

    return result;
}

std::optional<db::model::CompanyFinancials> FinancialService::getCompanyFinancials(const std::string &cik,
                                                                                   const std::string &period,
                                                                                   std::vector<std::string> &facts) {
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

    std::unordered_set<std::string> normalizedFacts = normalizeFacts(facts);
    result.reports = getFinancialReports(filings, period, facts, normalizedFacts);
    return result;
}

std::unordered_set<std::string> FinancialService::normalizeFacts(std::vector<std::string> &facts) {
    std::unordered_set<std::string> result;
    for (auto &fact : facts) {
        std::transform(fact.begin(), fact.end(), fact.begin(), ::tolower);
        auto it = factMap.find(fact);
        if (it != factMap.end()) {
            std::vector<std::string> associatedFacts = it->second;
            for (const auto &af : associatedFacts) {
                result.insert(af);
            }
        }
    }
    return result;
}

std::vector<db::model::FinancialReport>
FinancialService::getFinancialReports(std::vector<db::model::Filing> filings, std::string period,
                                      std::vector<std::string> &originalFacts,
                                      std::unordered_set<std::string> &normalizedFacts) {
    std::vector<db::model::FinancialReport> result;

    for (const auto &filing : filings) {
        db::model::FinancialReport financialReport;
        financialReport.filing = filing;
        std::vector<db::model::FinancialFact> facts =
            normalizedFacts.size() > 0
                ? factRepo.getFilteredByFilingIdAndFacts(filing.filingId, period, normalizedFacts)
                : factRepo.getAllFilteredByFilingId(filing.filingId, period);

        financialReport.stockPrice = stockRepo.getByFilingId(filing.filingId);
        constructFinancialReportFacts(financialReport, originalFacts, facts);
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

void FinancialService::constructFinancialReportFacts(db::model::FinancialReport &financialReport,
                                                     std::vector<std::string> &originalFacts,
                                                     std::vector<db::model::FinancialFact> &financialFacts) {
    if (originalFacts.size() == 0) {
        financialReport.facts = financialFacts;
        std::optional<db::model::FinancialFact> peFact = derivePeFact(financialFacts, financialReport.stockPrice);
        if (peFact != std::nullopt) {
            financialReport.facts.push_back(peFact.value());
        }
        return;
    }
    std::vector<db::model::FinancialFact> resultFacts;
    std::vector<const db::model::FinancialFact *> selectedFacts;
    for (const auto &oFact : originalFacts) {
        auto it = std::find_if(financialFacts.begin(), financialFacts.end(), [&](const db::model::FinancialFact &ff) {
            std::string tagLower = ff.tag;
            std::transform(tagLower.begin(), tagLower.end(), tagLower.begin(), ::tolower);
            return tagLower == oFact;
        });
        if (it != financialFacts.end()) {
            selectedFacts.push_back(&(*it));
        } else if (oFact == "pe ratio") {
            std::optional<db::model::FinancialFact> peFact = derivePeFact(financialFacts, financialReport.stockPrice);
            if (peFact != std::nullopt) {
                resultFacts.push_back(std::move(peFact.value()));
            }
        }
    }
    for (auto f : selectedFacts) {
        resultFacts.push_back(std::move(*f));
    }

    financialReport.facts = resultFacts;
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
std::optional<db::model::FinancialFact>
FinancialService::derivePeFact(const std::vector<db::model::FinancialFact> &facts,
                               std::optional<db::model::StockPrice> &stockPrice) {
    std::optional<db::model::FinancialFact> epsFact = std::nullopt;
    std::optional<db::model::FinancialFact> result = std::nullopt;
    for (const auto &f : facts) {
        if (f.sourceTag == "EarningsPerShareDiluted") {
            epsFact = f;
            break;
        }
    }
    if (epsFact.has_value() && stockPrice.has_value()) {
        std::string epsCurrency = getEPSCurrency(epsFact->unit);
        if (epsCurrency != stockPrice->currency)
            return result;
        double eps = epsFact->value;
        double pe = stockPrice->close / eps;
        db::model::FinancialFact peFact;
        peFact.filingId = epsFact->filingId;
        peFact.endDate = epsFact->endDate;
        peFact.value = pe;
        peFact.tag = "PE Ratio";
        result = peFact;
    }
    return result;
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
