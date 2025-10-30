#pragma once

#include "../models.h"
#include "page.h"
#include <ncurses.h>
#include <rapidfuzz/fuzz.hpp>
#include <string>
#include <vector>

class Application;

class SearchPage : public Page {
  public:
    explicit SearchPage(Application &app);
    void render() override;
    void handleInput(int ch) override;
    bool needsRefresh() const override;

    bool isCompanySelected() const { return selected; }
    Company &selectedCompany() { return company; }

  private:
    Application &app;
    std::string query;
    std::vector<Company> companies;
    bool refreshNeeded;
    int companyListSize;

    int selectedIndex = 0;
    bool selected = false;
    Company company;

    std::vector<Company> filterCompanies() const;
    double scoreCompany(const std::string &queryLower, const Company &c) const;
};
