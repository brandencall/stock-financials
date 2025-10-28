#pragma once

#include "../models.h"
#include "page.h"
#include <vector>

class Application;

class SearchPage : public Page {
  public:
    explicit SearchPage(Application &app);
    void render() override;
    void handleInput(int ch) override;

    bool isCompanySelected() const { return selected; }
    Company &selectedCompany() { return company; }

  private:
    Application &app;
    std::string query;
    std::vector<Company> companies;
    int selectedIndex = 0;
    bool selected = false;
    Company company;

    std::vector<Company> filterCompanies() const;
};
