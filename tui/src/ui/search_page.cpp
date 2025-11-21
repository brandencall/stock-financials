#include "ui/search_page.h"
#include "application.h"
#include <algorithm>
#include <cctype>
#include <ncurses.h>
#include <queue>
#include <rapidfuzz/fuzz.hpp>
#include <vector>

SearchPage::SearchPage(Application &app) : app(app), companyListSize(LINES - 2) {
    clear();
    printw("Getting companies...");
    refresh();
    companies = app.getCompanies();
}

void SearchPage::render() {
    mvprintw(0, 0, "Search: %s", query.c_str());

    auto filtered = filterCompanies();
    int maxDisplay = std::min((int)filtered.size(), companyListSize);

    for (int i = 0; i < maxDisplay; ++i) {
        if (i == selectedIndex) {
            attron(A_REVERSE);
            selectedCompany = filtered[i];
        }
        mvprintw(i + 2, 2, "%s - %s", filtered[i].ticker.c_str(), filtered[i].title.c_str());
        if (i == selectedIndex)
            attroff(A_REVERSE);
    }
    refreshNeeded = false;
}

void SearchPage::handleInput(int ch) {
    switch (ch) {
    case KEY_UP:
        if (selectedIndex > 0) {
            selectedIndex--;
            refreshNeeded = true;
        }
        break;
    case KEY_DOWN:
        if (selectedIndex < companyListSize - 1) {
            selectedIndex++;
            refreshNeeded = true;
        }
        break;
    case 10: // Enter
        app.switchToCompanyPage(selectedCompany);
        break;
    case KEY_BACKSPACE:
    case 127:
    case 8:
        if (!query.empty()) {
            query.pop_back();
            refreshNeeded = true;
        }
        selectedIndex = 0;
        break;
    default:
        if (isprint(ch)) {
            query += static_cast<char>(ch);
            selectedIndex = 0;
            refreshNeeded = true;
        }
        break;
    }
}

std::vector<Company> SearchPage::filterCompanies() const {
    if (query.empty())
        return companies;

    auto cmp = [](const auto &a, const auto &b) { return a.second < b.second; };
    std::priority_queue<std::pair<Company, double>, std::vector<std::pair<Company, double>>, decltype(cmp)> scored(cmp);

    std::string queryLower = query;
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);
    for (const auto &c : companies) {
        double score = scoreCompany(queryLower, c);
        if ((int)scored.size() < companyListSize) {
            scored.emplace(c, score);
        } else if (score > scored.top().second) {
            scored.pop();
            scored.emplace(c, score);
        }
    }

    std::vector<Company> result(companyListSize);
    for (int i = 0; i < companyListSize || !scored.empty(); ++i) {
        result[i] = scored.top().first;
        scored.pop();
    }

    return result;
}

double SearchPage::scoreCompany(const std::string &queryLower, const Company &c) const {
    using namespace rapidfuzz::fuzz;

    std::string tickerLower = c.ticker;
    std::transform(tickerLower.begin(), tickerLower.end(), tickerLower.begin(), ::tolower);
    std::string titleLower = c.title;
    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::tolower);

    int scoreTicker = rapidfuzz::fuzz::ratio(queryLower, tickerLower);
    int scoreTitle = rapidfuzz::fuzz::token_set_ratio(queryLower, titleLower);
    return std::max(scoreTicker, scoreTitle);
}

bool SearchPage::needsRefresh() const { return refreshNeeded; }
