#include "ui/company_page.h"
#include "application.h"
#include "models.h"
#include <functional>
#include <iomanip>
#include <ncurses.h>
#include <sstream>

CompanyPage::CompanyPage(Application &app, const Company &company) : app(app), company(company), refreshNeeded(false) {
    clear();
    printw("Fetching data...");
    refresh();
    companyData = app.getApiClient().getCompaniesAnnualFinancials(company.cik, 5);
    std::sort(
        companyData.reports.begin(), companyData.reports.end(),
        [](const FinancialReport &a, const FinancialReport &b) { return a.filing.filedDate > b.filing.filedDate; });
}

void CompanyPage::render() {
    mvprintw(0, 0, "This is company page for: %s", company.title.c_str());
    int line = 2;
    for (int i = 0; i < companyData.reports.size(); ++i) {
        FinancialReport report = companyData.reports[i];
        mvprintw(line, 2, "%s - %s - %s", report.filing.form.c_str(), std::to_string(report.filing.fy).c_str(),
                 report.filing.fp.c_str());
        line++;
        for (int j = 0; j < report.facts.size(); ++j) {
            FinancialFact fact = report.facts[j];
            if (fact.tag == "Revenue") {
                double revenue = fact.value;
                std::ostringstream oss;
                oss.imbue(std::locale(""));
                oss << std::fixed << std::setprecision(2) << revenue;
                mvprintw(line, 2, "Revenue: %s", oss.str().c_str());
                line++;
            }
        }
    }
}

void CompanyPage::handleInput(int ch) {}

bool CompanyPage::needsRefresh() const { return refreshNeeded; }
