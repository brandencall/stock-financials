#include "application.h"
#include "ui/compnay_page.h"

CompanyPage::CompanyPage(Application &app, Company &company) : app(app), company(company) {
    // Need to call out to the api here
}

void CompanyPage::render() {
    printw("This is the search page");
    refresh();
}

void CompanyPage::handleInput(int ch) {
    printw("%s", std::to_string(ch).c_str());
    refresh();
}
