#include "ui/company_page.h"
#include "application.h"
#include "models.h"
#include <ncurses.h>

CompanyPage::CompanyPage(Application &app, const Company &company) : app(app), company(company), refreshNeeded(false) {
    clear();
    printw("Fetching data...");
    refresh();
    companyData = app.getApiClient().getCompaniesAnnualFinancials(company.cik, 1);
}

void CompanyPage::render() {
    mvprintw(0, 0, "This is company page for: %s", company.title.c_str());
    int line = 2;
    for (int i = 0; i < companyData.reports.size(); ++i) {
        FinancialReport report = companyData.reports[i];
        mvprintw(line, 2, "%s - %s", report.filing.form.c_str(), report.filing.fp.c_str());
        line++;
        for (int j = 0; j < report.facts.size(); ++j) {
            FinancialFact fact = report.facts[j];
            mvprintw(line, 2, "%s : %s", fact.tag.c_str(), std::to_string(fact.value).c_str());
            line++;
        }
    }
}

void CompanyPage::handleInput(int ch) {}

bool CompanyPage::needsRefresh() const { return refreshNeeded; }
