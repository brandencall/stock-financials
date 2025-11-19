#include "ui/company_page.h"
#include "application.h"
#include "models.h"
#include "ui/ui_utils.h"
#include <format>
#include <ncurses.h>

CompanyPage::CompanyPage(Application &app, const Company &company) : app(app), company(company), refreshNeeded(false) {
    clear();
    printw("Fetching data...");
    refresh();
    financialFactList = {"Revenue",
                         "Operating Income",
                         "Earnings Per Share Diluted",
                         "PE Ratio",
                         "Cash And Cash Equivalents",
                         "Overall Debt",
                         "Current Debt",
                         "Long Term Debt"};

    companyData = app.getApiClient().getCompaniesFinancials(company.cik, "annual", financialFactList, 5);
    latestCompanyData = app.getApiClient().getCompaniesFinancials(company.cik, "annual", 1);

    mainWin = newwin(0, 0, 0, 0);

    headerWin = derwin(mainWin, headerHeight, headerWidth, headerY, headerX);
    revenueBarWin = derwin(mainWin, revenueBarHeight, revenueBarWidth, revenueBarY, revenueBarX);
    operatingIncomeWin =
        derwin(mainWin, operatingIncomeHeight, operatingIncomeWidth, operatingIncomeY, operatingIncomeX);
    epsWin = derwin(mainWin, epsHeight, epsWidth, epsY, epsX);
    peWin = derwin(mainWin, peHeight, peWidth, peY, peX);
    cashToDebtWin = derwin(mainWin, cashToDebtHeight, cashToDebtWidth, cashToDebtY, cashToDebtX);
}

CompanyPage::~CompanyPage() {
    delwin(mainWin);
    delwin(headerWin);
    delwin(revenueBarWin);
    delwin(operatingIncomeWin);
    delwin(epsWin);
    delwin(peWin);
    delwin(cashToDebtWin);
}

void CompanyPage::render() {
    box(mainWin, 1, 0);
    renderMainWindowTitle();

    if (companyData.reports.empty()) {
        renderNoDataError();
        wrefresh(mainWin);
        return;
    }

    box(revenueBarWin, 1, 0);
    box(operatingIncomeWin, 1, 0);
    box(epsWin, 1, 0);
    box(peWin, 1, 0);
    box(cashToDebtWin, 1, 0);

    renderHeader();
    renderBarGraphs();
    renderCashToDebtBar();

    wrefresh(mainWin);
    wrefresh(headerWin);
}

void CompanyPage::renderBarGraphs() {
    std::vector<DataPoint> revenuePoints;
    std::vector<DataPoint> operatingIncomePoints;
    std::vector<DataPoint> epsPoints;
    std::vector<DataPoint> pePoints;
    std::string currency;
    std::string epsUnit;
    for (long unsigned int i = 0; i < companyData.reports.size(); ++i) {
        FinancialReport report = companyData.reports[i];
        for (long unsigned int j = 0; j < report.facts.size(); ++j) {
            FinancialFact fact = report.facts[j];
            currency = currency.empty() ? fact.unit : currency;
            if (fact.tag == "Revenue") {
                double growth = 0;
                if (!revenuePoints.empty()) {
                    growth = calculateGrowthRate(fact.value, revenuePoints.back().value);
                }
                revenuePoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            } else if (fact.tag == "Operating Income") {
                double growth = 0;
                if (!operatingIncomePoints.empty()) {
                    growth = calculateGrowthRate(fact.value, operatingIncomePoints.back().value);
                }
                operatingIncomePoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            } else if (fact.tag == "Earnings Per Share Diluted") {
                double growth = 0;
                epsUnit = epsUnit.empty() ? fact.unit : epsUnit;
                if (!epsPoints.empty()) {
                    growth = calculateGrowthRate(fact.value, epsPoints.back().value);
                }
                epsPoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            } else if (fact.tag == "PE Ratio") {
                double growth = 0;
                if (!pePoints.empty()) {
                    growth = calculateGrowthRate(fact.value, pePoints.back().value);
                }
                pePoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            }
        }
    }
    renderRevenue(revenuePoints, currency);
    renderOperatingIncome(operatingIncomePoints, currency);
    renderEPS(epsPoints, epsUnit);
    renderPe(pePoints);
}

double CompanyPage::calculateGrowthRate(double currentValue, double previousValue) {
    if (previousValue == 0)
        return 0.0;
    return ((currentValue - previousValue) / std::abs(previousValue)) * 100.0;
}

void CompanyPage::renderRevenue(const std::vector<DataPoint> &revenuePoints, const std::string &currency) {
    std::string title = "Revenue (" + currency + ")";
    UiUtils::renderWindowTitle(revenueBarWin, title, 1);
    UiUtils::renderHorizontalBarChart(revenueBarWin, revenuePoints, 0.65, 1, 2);
    wrefresh(revenueBarWin);
}

void CompanyPage::renderOperatingIncome(const std::vector<DataPoint> &operatingIncomePoints,
                                        const std::string &currency) {
    std::string title = "Operating Income (" + currency + ")";
    UiUtils::renderWindowTitle(operatingIncomeWin, title, 1);
    UiUtils::renderHorizontalBarChart(operatingIncomeWin, operatingIncomePoints, 0.65, 1, 2);
    wrefresh(operatingIncomeWin);
}
void CompanyPage::renderEPS(const std::vector<DataPoint> &epsPoints, const std::string &unit) {
    std::string title = "EPS (" + unit + ")";
    UiUtils::renderWindowTitle(epsWin, title, 1);
    UiUtils::renderHorizontalBarChart(epsWin, epsPoints, 0.45, 1, 2);
    wrefresh(epsWin);
}
void CompanyPage::renderPe(const std::vector<DataPoint> &pePoints) {
    std::string title = "PE Ratio";
    UiUtils::renderWindowTitle(peWin, title, 1);
    UiUtils::renderHorizontalBarChart(peWin, pePoints, 0.45, 1, 2);
    wrefresh(peWin);
}

void CompanyPage::renderCashToDebtBar() {
    double cash = 0;
    double debt = 0;
    if (latestCompanyData.reports.empty())
        return;
    FinancialReport lastReport = latestCompanyData.reports[0];
    for (const auto &fact : lastReport.facts) {
        if (fact.tag == "Cash And Cash Equivalents") {
            cash = fact.value;
        } else if (fact.tag == "Overall Debt") {
            debt = fact.value;
        }
    }
    if (debt == 0) {
        for (const auto &fact : lastReport.facts) {
            if (fact.tag == "Current Debt") {
                debt += fact.value;
            } else if (fact.tag == "Long Term Debt") {
                debt += fact.value;
            }
        }
    }

    std::string title = "Cash To Debt";
    UiUtils::renderWindowTitle(cashToDebtWin, title, 1);
    UiUtils::renderCashBalanceBar(cashToDebtWin, cash, debt, 2, 1);
    wrefresh(cashToDebtWin);
}

void CompanyPage::renderMainWindowTitle() {
    std::string title = " " + company.title + " (" + company.ticker + ") ";
    UiUtils::renderWindowTitle(mainWin, title, UiUtils::getCenterForString(mainWin, title));
}

void CompanyPage::renderNoDataError() {
    std::string errorMsg = "There is no financial data for this company :(";
    wattron(mainWin, A_BOLD);
    mvwprintw(mainWin, 2, UiUtils::getCenterForString(mainWin, errorMsg), "%s", errorMsg.c_str());
    wattroff(mainWin, A_BOLD);
}

void CompanyPage::renderHeader() {
    CompanyHeaderData data = getHeaderData();

    std::string header =
        std::format("Price: ${:.2f}  |  P/E: {:.2f}  |  Dividend: ${:.2f}  |  Market Cap: ${:s}", data.price,
                    data.peRatio, data.dividend, UiUtils::abbreviateNumber(data.marketCap));

    mvwprintw(headerWin, 2, UiUtils::getCenterForString(headerWin, header), "%s", header.c_str());
}

CompanyHeaderData CompanyPage::getHeaderData() {
    CompanyHeaderData result;
    FinancialReport latestReport = companyData.reports[companyData.reports.size() - 1];
    result.fiscalYear = "FY" + std::to_string(latestReport.filing.fy);
    result.price = latestReport.stockPrice.close;
    for (const auto &fact : latestReport.facts) {
        if (fact.tag == "PE Ratio")
            result.peRatio = fact.value;
        if (fact.tag == "Dividens Per Share")
            result.dividend = fact.value;
        if (fact.tag == "Shares") {
            result.marketCap = fact.value * result.price;
        }
    }
    return result;
};

bool CompanyPage::needsRefresh() const { return refreshNeeded; }
