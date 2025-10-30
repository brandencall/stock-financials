#include "ui/search_page.h"
#include "application.h"
#include <algorithm>
#include <cctype>
#include <ncurses.h>
#include <rapidfuzz/fuzz.hpp>

SearchPage::SearchPage(Application &app) : app(app), refreshNeeded(true), companyListSize(LINES - 2) {
    companies = app.getCompanies();
}

void SearchPage::render() {
    mvprintw(0, 0, "Search: %s", query.c_str());

    auto filtered = filterCompanies();
    int maxDisplay = std::min((int)filtered.size(), companyListSize);

    for (int i = 0; i < maxDisplay; ++i) {
        if (i == selectedIndex)
            attron(A_REVERSE);
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
        // In future: return selected company to application
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

    std::vector<std::pair<Company, int>> scored(companies.size());
    std::string queryLower = query;
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);
    for (const auto &c : companies) {
        int score = rapidfuzz::fuzz::partial_ratio(queryLower, c.queryString);
        scored.emplace_back(c, score);
    }

    std::sort(scored.begin(), scored.end(), [](auto &a, auto &b) { return a.second > b.second; });

    std::vector<Company> result(companyListSize);
    for (int i = 0; i < companyListSize; ++i) {
        result[i] = scored[i].first;
    }
    // std::vector<Company> result;
    // for (auto &[company, _] : scored)
    //     result.push_back(company);

    return result;
}

bool SearchPage::needsRefresh() const { return refreshNeeded; }
