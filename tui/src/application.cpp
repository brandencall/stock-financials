#include "application.h"
#include "ui/compnay_page.h"
#include "ui/search_page.h"
#include <ncurses.h>
#include <thread>

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
            refresh();
        } else {
            currentPage->handleInput(ch);
        }

        //if (auto *search = dynamic_cast<SearchPage *>(currentPage.get())) {
        //    if (search->isCompanySelected()) {
        //        switchToCompanyPage(search->selectedCompany());
        //    }
        //}
    }
}

void Application::shutdownUi() { endwin(); }

void Application::switchToSearchPage() { currentPage = std::make_unique<SearchPage>(*this); }

void Application::switchToCompanyPage(Company &selectedCompany) {
    currentPage = std::make_unique<CompanyPage>(*this, selectedCompany);
}

const std::vector<Company> &Application::getCompanies() {
    if (!companiesLoaded) {
        cachedCompanies = apiClient.getCompanies();
        companiesLoaded = true;
    }
    return cachedCompanies;
}
