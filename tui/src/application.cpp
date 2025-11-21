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

    switchPage(std::make_unique<SearchPage>(*this));
}

void Application::run() {
    nodelay(stdscr, FALSE);
    int ch;

    while ((ch = getch()) != 'q') {
        currentPage->handleInput(ch);

        if (ch == '/') {
            switchPage(std::make_unique<SearchPage>(*this));
        }

        if (currentPage->refreshNeeded) {
            clear();
            currentPage->render();
        }
    }
}

void Application::shutdownUi() { endwin(); }

void Application::switchPage(std::unique_ptr<Page> page) {
    currentPage = std::move(page);
    currentPage->refreshNeeded = true;
    currentPage->render();
}

const std::vector<Company> &Application::getCompanies() {
    if (!companiesLoaded) {
        cachedCompanies = apiClient.getCompanies();
        companiesLoaded = true;
    }
    return cachedCompanies;
}
