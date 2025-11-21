#include "ui/filings_page.h"
#include "models.h"

FilingsPage::FilingsPage(Application &app, const Company &company) : app(app), company(company) {}

void FilingsPage::render() {
    printw("In the filings page");
    refresh();
}

void FilingsPage::handleInput(int ch) {}
