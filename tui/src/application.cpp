#include "application.h"
#include "ui/company_page.h"
#include "ui/search_page.h"
#include <ncurses.h>

Application::Application(ApiClient &apiClient) : apiClient(apiClient) {}

void Application::initUi() {
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo typed characters
    keypad(stdscr, TRUE); // Enable arrow keys

    switchToSearchPage();
}

void Application::run() {
    nodelay(stdscr, TRUE);
    int ch;

    while ((ch = getch()) != 'q') {
        if (currentPage->needsRefresh()) {
            clear();
            currentPage->handleInput(ch);
            currentPage->render();
        } else {
            currentPage->handleInput(ch);
        }

        if (ch == '/') {
            switchToSearchPage();
        }
    }
}

void Application::shutdownUi() { endwin(); }

void Application::switchToSearchPage() { currentPage = std::make_unique<SearchPage>(*this); }

void Application::switchToCompanyPage(const Company &selectedCompany) {
    currentPage = std::make_unique<CompanyPage>(*this, selectedCompany);
    //clear();
    currentPage->render();
}

const std::vector<Company> &Application::getCompanies() {
    if (!companiesLoaded) {
        cachedCompanies = apiClient.getCompanies();
        companiesLoaded = true;
    }
    return cachedCompanies;
}
