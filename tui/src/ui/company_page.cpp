#include "ui/company_page.h"
#include "application.h"
#include <ncurses.h>

CompanyPage::CompanyPage(Application &app, const Company &company) : app(app), company(company), refreshNeeded(false) {
    clear();
    printw("Fetching data...");
    refresh();
    companyData = app.getApiClient().getCompaniesAnnualFinancials(company.cik, 5);
}

void CompanyPage::render() {
    mvprintw(0, 0, "This is company page for: %s", company.title.c_str());
    mvprintw(1, 0, "This is data: %s", companyData.c_str());
}

void CompanyPage::handleInput(int ch) {}

bool CompanyPage::needsRefresh() const { return refreshNeeded; }
