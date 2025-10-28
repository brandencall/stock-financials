#include "ui/search_page.h"
#include "application.h"
#include <ncurses.h>
#include <string>

SearchPage::SearchPage(Application &app) : app(app) { companies = app.getCompanies(); }

void SearchPage::render() {
    mvprintw(0, 0, "Search: %s", query.c_str());

    auto filtered = filterCompanies();
    int maxDisplay = std::min((int)filtered.size(), LINES - 2);

    for (int i = 0; i < maxDisplay; ++i) {
        if (i == selectedIndex)
            attron(A_REVERSE);
        mvprintw(i + 2, 2, "%s - %s", filtered[i].ticker.c_str(), filtered[i].title.c_str());
        if (i == selectedIndex)
            attroff(A_REVERSE);
    }
}

void SearchPage::handleInput(int ch) {
    switch (ch) {
    case KEY_UP:
        if (selectedIndex > 0)
            selectedIndex--;
        break;
    case KEY_DOWN:
        if (selectedIndex < (int)filterCompanies().size() - 1)
            selectedIndex++;
        break;
    case 10: // Enter
        // In future: return selected company to application
        break;
    case KEY_BACKSPACE:
    case 127:
        if (!query.empty())
            query.pop_back();
        selectedIndex = 0;
        break;
    default:
        if (isprint(ch)) {
            query += static_cast<char>(ch);
            selectedIndex = 0;
        }
        break;
    }
}

std::vector<Company> SearchPage::filterCompanies() const {
    std::vector<Company> results;
    for (const auto &c : companies) {
        std::string lowerTitle = c.title;
        std::string lowerQuery = query;

        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        if (lowerTitle.find(lowerQuery) != std::string::npos || c.ticker.find(query) != std::string::npos)
            results.push_back(c);
    }
    return results;
}
