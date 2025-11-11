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
    companyData = app.getApiClient().getCompaniesAnnualFinancials(company.cik, 5);

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
    renderTitle();

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
    renderRevenueBar();
    renderOperatingIncomeBar();
    renderEpsBar();
    renderPeBar();
    renderCashToDebtBar();

    wrefresh(mainWin);
    wrefresh(headerWin);
    wrefresh(revenueBarWin);
    wrefresh(operatingIncomeWin);
    wrefresh(epsWin);
    wrefresh(peWin);
    wrefresh(cashToDebtWin);
}

void CompanyPage::renderRevenueBar() {
    std::vector<DataPoint> revenuePoints;
    std::string currency;
    for (int i = 0; i < companyData.reports.size(); ++i) {
        FinancialReport report = companyData.reports[i];
        for (int j = 0; j < report.facts.size(); ++j) {
            FinancialFact fact = report.facts[j];
            if (fact.tag == "Revenue") {
                currency = currency.empty() ? fact.unit : currency;
                double revenue = fact.value;
                double growth = 0;
                if (i > 0) {
                    growth = ((revenue - revenuePoints[i - 1].value) / revenuePoints[i - 1].value) * 100.0;
                }
                revenuePoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            }
        }
    }
    std::string title = "Revenue (" + currency + ")";
    wattron(revenueBarWin, A_BOLD);
    mvwprintw(revenueBarWin, 0, 1, " %s ", title.c_str());
    wattroff(revenueBarWin, A_BOLD);
    UiUtils::renderHorizontalBarChart(revenueBarWin, revenuePoints, 0.65, 1, 2);
}

void CompanyPage::renderOperatingIncomeBar() {
    std::vector<DataPoint> operatingIncomePoints;
    std::string currency;
    for (int i = 0; i < companyData.reports.size(); ++i) {
        FinancialReport report = companyData.reports[i];
        for (int j = 0; j < report.facts.size(); ++j) {
            FinancialFact fact = report.facts[j];
            if (fact.tag == "Operating Income") {
                currency = currency.empty() ? fact.unit : currency;
                double operatingIncome = fact.value;
                double growth = 0;
                if (i > 0) {
                    growth = ((operatingIncome - operatingIncomePoints[i - 1].value) /
                              std::abs(operatingIncomePoints[i - 1].value)) *
                             100.0;
                }
                operatingIncomePoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            }
        }
    }
    std::string title = "Operating Income (" + currency + ")";
    wattron(operatingIncomeWin, A_BOLD);
    mvwprintw(operatingIncomeWin, 0, 1, " %s ", title.c_str());
    wattroff(operatingIncomeWin, A_BOLD);
    UiUtils::renderHorizontalBarChart(operatingIncomeWin, operatingIncomePoints, 0.65, 1, 2);
}

void CompanyPage::renderEpsBar() {
    std::vector<DataPoint> epsPoints;
    std::string currency;
    for (int i = 0; i < companyData.reports.size(); ++i) {
        FinancialReport report = companyData.reports[i];
        for (int j = 0; j < report.facts.size(); ++j) {
            FinancialFact fact = report.facts[j];
            if (fact.tag == "Earnings Per Share Diluted") {
                currency = currency.empty() ? fact.unit : currency;
                double eps = fact.value;
                double growth = 0;
                if (i > 0) {
                    growth = ((eps - epsPoints[i - 1].value) / std::abs(epsPoints[i - 1].value)) * 100.0;
                }
                epsPoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            }
        }
    }
    std::string title = "EPS Diluted (" + currency + ")";
    wattron(epsWin, A_BOLD);
    mvwprintw(epsWin, 0, 1, " %s ", title.c_str());
    wattroff(epsWin, A_BOLD);
    UiUtils::renderHorizontalBarChart(epsWin, epsPoints, 0.45, 1, 2);
}

void CompanyPage::renderPeBar() {
    std::vector<DataPoint> pePoints;
    for (int i = 0; i < companyData.reports.size(); ++i) {
        FinancialReport report = companyData.reports[i];
        for (int j = 0; j < report.facts.size(); ++j) {
            FinancialFact fact = report.facts[j];
            if (fact.tag == "PE Ratio") {
                double pe = fact.value;
                double growth = 0;
                if (i > 0 && pePoints[i - 1].value > 0) {
                    growth = ((pe - pePoints[i - 1].value) / pePoints[i - 1].value) * 100.0;
                }
                pePoints.emplace_back(std::to_string(report.filing.fy), fact.value, growth);
            }
        }
    }
    std::string title = "PE Ratio";
    wattron(peWin, A_BOLD);
    mvwprintw(peWin, 0, 1, " %s ", title.c_str());
    wattroff(peWin, A_BOLD);
    UiUtils::renderHorizontalBarChart(peWin, pePoints, 0.45, 1, 2);
}

void CompanyPage::renderCashToDebtBar() {
    double cash = 0;
    double debt = 0;
    FinancialReport lastReport = companyData.reports[companyData.reports.size() - 1];
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
    wattron(cashToDebtWin, A_BOLD);
    mvwprintw(cashToDebtWin, 0, 1, " %s ", title.c_str());
    wattroff(cashToDebtWin, A_BOLD);
    UiUtils::renderCashBalanceBar(cashToDebtWin, cash, debt, 2, 1);
}

void CompanyPage::renderTitle() {
    std::string title = company.title + " (" + company.ticker + ")";
    wattron(mainWin, A_BOLD);
    mvwprintw(mainWin, 0, UiUtils::getCenterForString(mainWin, title), " %s ", title.c_str());
    wattroff(mainWin, A_BOLD);
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
    int width = (getmaxx(headerWin) / 2) - (header.size() / 2);

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

void CompanyPage::handleInput(int ch) {}

bool CompanyPage::needsRefresh() const { return refreshNeeded; }
