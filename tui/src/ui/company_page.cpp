#include "ui/company_page.h"
#include "application.h"

CompanyPage::CompanyPage(Application &app, const Company &company) : app(app), company(company), refreshNeeded(false) {
    // Need to call out to the api here
}

void CompanyPage::render() { mvprintw(0, 0, "This is company page for: %s", company.title.c_str()); }

void CompanyPage::handleInput(int ch) {}

bool CompanyPage::needsRefresh() const { return refreshNeeded; }
